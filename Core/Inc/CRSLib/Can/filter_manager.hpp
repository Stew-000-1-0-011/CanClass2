// フィルタの管理を行う.
// 実はびっくりするくらい雑. フィルタを登録できるが解除することはできない.
// でも解除できたほうが面白いか？

#pragma once

#include <tuple>
#include <concepts>
#include <type_traits>
#include <array>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/debug.hpp>
#include "utility.hpp"
#include "config.hpp"
#include "rx_unit.hpp"

namespace CRSLib::Can
{
	// なんでDualCANはFilterBankを共有してんだ！？
	namespace FilterManager
	{
		template<CanX can_x>
		inline constexpr u32 max_filter_bank_index = (can_x == CanX::can1)? Config::slave_start_filter_bank - 1 : Config::filter_bank_total_size - Config::slave_start_filter_bank + 1;

		namespace Implement::FilterManagerImp
		{
			using namespace IntegerLiterals;

			inline constexpr u32 lshift_to_high_std_id = 5;
			inline constexpr u32 rshift_to_high_ext_id = 13;
			inline constexpr u32 lshift_to_low_ext_id = 3;
			inline constexpr u32 ext_id_0_12_bit = 0x1F'FF;
			inline constexpr u32 high_ext_id_bit = 0x1F;
			inline constexpr u32 ide_bit = 0x4;

			inline constexpr u32 get_id(const CAN_FilterTypeDef& filter) noexcept
			{
				return (filter.FilterIdHigh & high_ext_id_bit) | filter.FilterIdLow & (ext_id_0_12_bit << lshift_to_high_std_id);
			}

			template<CanX can_x, IsOffsetIdsEnum OffsetIdsEnum>
			inline u32 get_filter_bank_index(const RxUnit<OffsetIdsEnum>& rx_unit) noexcept
			{
				for(u32 i = 0; i < Config::filter_bank_total_size; ++i)
				{
					if(rx_unit.get_base_id() == get_id(filter_banks<can_x>[i]) && filter_banks<can_x>[i].FilterFIFOAssignment == convert_fifo_index_to_hal_can_filter_fifo<fifo_index>())
					{
						return i;
					}
				}
				return -1;
			}

			inline constexpr CAN_FilterTypeDef make_can_filter(const u32 base_id, const u32 align, const FifoIndex fifo_index, const u32 filter_bank_index, bool activated) noexcept
			{
				CAN_FilterTypeDef ret{};

				// align - 1とすると無視してよいビットが0となる.
				const u32 std_id_mask = max_std_id - (align - 1);
				const u32 ext_id_mask = max_ext_id - (align - 1);

				if(is_in_std_id_range(base_id))
				{
					// このときbase_id + alignは必ずmax_can_std_id以下である.
					// つまり、このCanUnitのもつIDは全て標準フレームに収まる.

					// IDの設定
					ret.FilterIdHigh = base_id << lshift_to_high_std_id;
					ret.FilterIdLow = base_id << lshift_to_low_ext_id;

					// マスクの設定. 0になっているビットは無視される.
					ret.FilterMaskIdHigh = std_id_mask << lshift_to_high_std_id;
					ret.FilterMaskIdLow = ext_id_mask << lshift_to_low_ext_id;
				}
				else
				{
					// ここではbase_idがmax_can_std_idより大きいので標準フレームは考えなくてよい.

					// IDの設定
					ret.FilterIdHigh = base_id >> rshift_to_high_ext_id;
					ret.FilterIdLow = (base_id & ext_id_0_12_bit) << lshift_to_low_ext_id | ide_bit;

					// マスクの設定
					ret.FilterMaskIdHigh = high_ext_id_bit;
					ret.FilterMaskIdLow = ext_id_0_12_bit << lshift_to_low_ext_id | ide_bit;
				}

				switch(fifo_index)
				{
				case FifoIndex::fifo0:
					ret.FilterFIFOAssignment = CAN_FILTER_FIFO0;
					break;
				
				case FifoIndex::fifo1:
					ret.FilterFIFOAssignment = CAN_FILTER_FIFO1;
				default:;
				}
				ret.FilterBank = filter_bank_index;
				ret.FilterMode = CAN_FILTERMODE_IDMASK;
				ret.FilterScale = CAN_FILTERSCALE_32BIT;
				ret.FilterActivation = activated ? CAN_FILTER_ENABLE : CAN_FILTER_DISABLE;
				ret.SlaveStartFilterBank = Config::slave_start_filter_bank;

				return ret;
			}

			inline constexpr CAN_FilterTypeDef make_null_filter(const u32 filter_bank_index) noexcept
			{
				return make_can_filter(null_id, 1_u32, FifoIndex::fifo0, filter_bank_index, false);
			}

			template<CanX can_x>
			consteval auto initialize_filter_banks() noexcept
			{
				std::array<CAN_FilterTypeDef, max_filter_bank_index<can_x>> ret;
				for(u32 i = 0; i < ret.size(); ++i)
				{
					ret[i] = make_null_filter(i);
				}

				return ret;
			}

			template<CanX can_x>
			auto filter_banks = initialize_filter_banks<can_x>();
		}

		template<CanX can_x>
		void register_can_filter(const RxUnit& rx_unit, const FifoIndex fifo_index, bool activated) noexcept
		{
			for(auto& e : Implement::FilterManagerImp::filter_banks<can_x>) if()
			{
				Debug::set_error("This filter already exists.");
				Error_Handler();
			}
			
			else if(const u32 empty_filter_bank_index = get_filter_bank_index(null_id); empty_filter_bank_index == -1)
			{
				Debug::set_error("Can't register filter more.");
				Error_Handler();
			}
			else
			{
				auto filter = make_can_filter(base_id, align, fifo_index, empty_filter_bank_index, activated);
				if(HAL_CAN_ConfigFilter(Config::can_instance<can_x>, &filter) != HAL_OK)
				{
					Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
					Error_Handler();
				}
			}
		}

		template<CanX can_x>
		void unregister_can_filter(const u32 base_id, const FifoIndex fifo_index) noexcept
		{
			const u32 filter_bank_index = Implement::FilterManagerImp::get_filter_bank_index(base_id, fifo_index);
			if(filter_bank_index == -1)
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}

			Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index] = Implement::FilterManagerImp::make_can_filter(null_id, 1_u32, (filter_bank_index % 2)? FifoIndex::fifo0 : FifoIndex::fifo1, filter_bank_index, false);
			if(HAL_CAN_ConfigFilter(Config::can_instance<can_x>, &Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index]) != HAL_OK)
			{
				Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
				Error_Handler();
				return;
			}
		}

		void dynamic_initialize() noexcept
		{
			if constexpr(Config::is_dual_can)
			{
				for(auto& e : Implement::FilterManagerImp::filter_banks<CanX::can1>) 
				{
					if(HAL_CAN_ConfigFilter(Config::can_instance<CanX::can1>, &e) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}
				}
				for(auto& e : Implement::FilterManagerImp::filter_banks<CanX::can2>)
				{
					if(HAL_CAN_ConfigFilter(Config::can_instance<CanX::can2>, &e) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}
				}
			}
			else
			{
				for(auto& e : Implement::FilterManagerImp::filter_banks<CanX::single_can>)
				{
					if(HAL_CAN_ConfigFilter(Config::can_instance<CanX::single_can>, &e) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}
				}
			}
		}

		template<CanX can_x>
		inline void plug_ears(const u32 base_id, const FifoIndex fifo_index) noexcept
		{
			const u32 filter_bank_index = Implement::FilterManagerImp::get_filter_bank_index(base_id, fifo_index);
			if(filter_bank_index == -1)
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}

			Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index].FilterActivation = CAN_FILTER_DISABLE;
			if(HAL_CAN_ConfigFilter(&Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index]) != HAL_OK)
			{
				Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
				Error_Handler();
				return;
			}
		}

		template<CanX can_x>
		inline void listen_to(const u32 base_id, const FifoIndex fifo_index) noexcept
		{
			const u32 filter_bank_index = Implement::FilterManagerImp::get_filter_bank_index(base_id, fifo_index);
			if(filter_bank_index == -1)
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}

			Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index].FilterActivation = CAN_FILTER_ENABLE;
			if(HAL_CAN_ConfigFilter(&Implement::FilterManagerImp::filter_banks<can_x>[filter_bank_index]) != HAL_OK)
			{
				Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
				Error_Handler();
				return;
			}
		}
	}
}
