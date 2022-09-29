#pragma once

#include <compare>
#include <optional>
#include <array>
#include <algorithm>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/debug.hpp>
#include "utility.hpp"
#include "config.hpp"
#include "handle.hpp"

namespace CRSLib::Can
{
	// なんでDualCANはFilterBankを共有してんだ！？
	namespace FilterManager
	{
		namespace Implement
		{
			template<CanX can_x>
			inline constexpr u32 max_filter_bank_index = (can_x == CanX::can1)?
				Config::slave_start_filter_bank - 1 :
				Config::filter_bank_total_size - Config::slave_start_filter_bank + 1;
		}

		struct FilterId final
		{
			u32 base_id{null_id};
			u32 align{1};
			FifoIndex fifo_index{FifoIndex::fifo0};

			constexpr bool is_in(const u32 id) noexcept
			{
				return base_id <= id && id < base_id + align;
			}

			friend auto operator<=>(const FilterId&, const FilterId&) = default;

			friend constexpr bool is_overlap(const FilterId& l, const FilterId& r) noexcept
			{
				return l.fifo_index == r.fifo_index && l.base_id < r.base_id + r.align && r.base_id < l.base_id + l.align;
			}
		};

		namespace Implement
		{
			inline constexpr u32 lshift_to_high_std_id = 5;
			inline constexpr u32 rshift_to_high_ext_id = 13;
			inline constexpr u32 lshift_to_low_ext_id = 3;
			inline constexpr u32 ext_id_0_12_bit = 0b1'1111'1111'1111;
			inline constexpr u32 high_ext_id_bit = 0b1'1111;
			inline constexpr u32 ide_bit = 0xb100;

			inline constexpr CAN_FilterTypeDef make_can_filter(const FilterId& filter_id, const u32 filter_bank_index, bool activated) noexcept
			{
				CAN_FilterTypeDef ret{};

				// align - 1とすると無視してよいビットが0となる.
				const u32 std_id_mask = max_std_id - (filter_id.align - 1);
				const u32 ext_id_mask = max_ext_id - (filter_id.align - 1);

				if(is_in_std_id_range(filter_id.base_id))
				{
					// このときbase_id + alignは必ずmax_std_id以下である.
					// つまり, このCanUnitのもつIDは全て標準フレームに収まる.

					// IDの設定
					ret.FilterIdHigh = filter_id.base_id << lshift_to_high_std_id;
					ret.FilterIdLow = filter_id.base_id << lshift_to_low_ext_id;

					// マスクの設定. 0になっているビットは無視される.
					ret.FilterMaskIdHigh = std_id_mask << lshift_to_high_std_id;
					ret.FilterMaskIdLow = ext_id_mask << lshift_to_low_ext_id;
				}
				else
				{
					// ここではbase_idがmax_std_idより大きいので標準フレームは考えなくてよい.

					// IDの設定
					ret.FilterIdHigh = filter_id.base_id >> rshift_to_high_ext_id;
					ret.FilterIdLow = (filter_id.base_id & ext_id_0_12_bit) << lshift_to_low_ext_id | ide_bit;

					// マスクの設定
					ret.FilterMaskIdHigh = high_ext_id_bit;
					ret.FilterMaskIdLow = ext_id_0_12_bit << lshift_to_low_ext_id | ide_bit;
				}

				ret.FilterFIFOAssignment = static_cast<u32>(filter_id.fifo_index);
				ret.FilterBank = filter_bank_index;
				ret.FilterMode = CAN_FILTERMODE_IDMASK;
				ret.FilterScale = CAN_FILTERSCALE_32BIT;
				ret.FilterActivation = activated ? CAN_FILTER_ENABLE : CAN_FILTER_DISABLE;
				ret.SlaveStartFilterBank = Config::slave_start_filter_bank;

				return ret;
			}

			template<CanX can_x>
			struct FilterBank final
			{
				struct FilterState
				{
					FilterId filter_id{};
					bool is_active{};
				};

				inline static constinit std::array<std::optional<FilterState>, max_filter_bank_index<can_x>> bank{};

				static constexpr bool has_overlapping_filter(const FilterId& filter_id) noexcept
				{
					return std::ranges::any_of(bank, [&filter_id](const auto& e) -> bool {return e ? is_overlap(e->filter_id, filter_id) : false;});
				}

				static constexpr auto find_if(const FilterId& filter_id) noexcept
				{
					return std::ranges::find_if(bank, [&filter_id](const auto& e) -> bool {return e ? e->filter_id == filter_id : false;});
				}

				static constexpr u32 iterator_to_index(const auto iter) noexcept
				{
					return iter - bank.begin();
				}

				static constexpr bool is_end(const auto iter) noexcept
				{
					return iter == bank.end();
				}
			};
		}

		template<CanX can_x>
		void register_filter(const FilterId& filter_id, bool activated) noexcept
		{
			using namespace Implement;

			if(is_overlap(filter_id, FilterId{}))
			{
				Debug::set_error("This filter contains null_id.");
				Error_Handler();
				return;
			}

			// 被りを許容.
			// (FMIがどうも怪しい, というか仕様書を見ても説明が足りないだろこれ. CAN_FFA1Rレジスタを変更すると
			// Filter Number(Filter Match Indexとも？)が軒並み変更されるってことか...?
			// わからなかったので今回は使用を見送ることにした.
			// FMIを使わないなら自分でidと格納先のキューを比較することになり,
			// そうなれば同じidに対してキューが複数個ある場合を扱うこともたやすい)
			// 
			// if(FilterBank<can_x>::has_overlapping_filter(filter_id))
			// {
			// 	Debug::set_error("This filter overlaps with registered one.");
			// 	Error_Handler();
			// 	return;
			// }

			if(const auto empty_iter = find_if<can_x>(FilterId{}); is_end(empty_iter))
			{
				Debug::set_error("Can't register filter more.");
				Error_Handler();
				return;
			}
			else
			{
				const u32 empty_index = FilterBank<can_x>::iterator_to_index(empty_iter);
				auto filter = make_can_filter(filter_id, empty_index, activated);
				if(HAL_CAN_ConfigFilter(&Can::Implement::hcan<can_x>, &filter) != HAL_OK)
				{
					Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
					Error_Handler();
				}
				*empty_iter = {filter_id, activated};
			}
		}

		template<CanX can_x>
		void unregister_filter(const FilterId& filter_id) noexcept
		{
			using namespace Implement;

			if(const auto unregistered_iter = FilterBank<can_x>::find_if<can_x>(filter_id); is_end(unregistered_iter))
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}
			else
			{
				const u32 unregistered_index = FilterBank<can_x>::iterator_to_index(unregistered_iter);
				auto filter = make_can_filter(filter_id, unregistered_index, false);
				if(HAL_CAN_ConfigFilter(&Can::Implement::hcan<can_x>, &filter) != HAL_OK)
				{
					Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
					Error_Handler();
					return;
				}

				*unregistered_iter = {FilterId{}, false};
			}
		}

		void dynamic_initialize() noexcept
		{
			using namespace Implement;
			using namespace IntegerLiterals;

			auto null_filter = make_can_filter(FilterId{}, /*undefined*/0_u32, false);

			if constexpr(Config::is_dual_can)
			{
				for(u32 i = 0; i < FilterBank<CanX::can1>::bank.size(); ++i) 
				{
					null_filter.FilterBank = i;

					if(HAL_CAN_ConfigFilter(&Can::Implement::hcan<CanX::can1>, &null_filter) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}

					++i;
				}
				for(u32 i = 0; i < FilterBank<CanX::can2>::bank.size(); ++i) 
				{
					null_filter.FilterBank = i;

					if(HAL_CAN_ConfigFilter(&Can::Implement::hcan<CanX::can2>, &null_filter) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}

					++i;
				}
			}
			else
			{
				for(u32 i = 0; i < FilterBank<CanX::single_can>::bank.size(); ++i)
				{
					null_filter.FilterBank = i;

					if(HAL_CAN_ConfigFilter(&Can::Implement::hcan<CanX::single_can>, &null_filter) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
						Error_Handler();
						return;
					}

					++i;
				}
			}
		}

		template<CanX can_x>
		inline void plug_ears(const FilterId& filter_id) noexcept
		{
			using namespace Implement;

			const auto disabled_iter = FilterBank<can_x>::find_if<can_x>(filter_id);
			if(FilterBank<can_x>::is_end(disabled_iter))
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}

			auto filter = make_can_filter(filter_id, FilterBank<can_x>::iterator_to_index(disabled_iter), false);
			if(HAL_CAN_ConfigFilter(&filter) != HAL_OK)
			{
				Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
				Error_Handler();
				return;
			}
			
			disabled_iter->is_active = false;
		}

		template<CanX can_x>
		inline void listen_to(const FilterId& filter_id) noexcept
		{
			using namespace Implement;

			const auto enabled_iter = find_if<can_x>(filter_id);
			if(FilterBank<can_x>::is_end(enabled_iter))
			{
				Debug::set_error("No Filter has this base_id and fifo_index.");
				Error_Handler();
				return;
			}

			auto filter = make_can_filter(filter_id, FilterBank<can_x>::iterator_to_index(enabled_iter), true);
			if(HAL_CAN_ConfigFilter(&filter) != HAL_OK)
			{
				Debug::set_error("Fail to call HAL_CAN_ConfigFilter.");
				Error_Handler();
				return;
			}

			enabled_iter->is_active = true;
		}
	}
}
