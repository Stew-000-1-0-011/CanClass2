#pragma once

#include <utility>
#include <type_traits>
#include <tuple>

#include <main.h>

#include <CRSLib/compile_for.hpp>
#include <CRSLib/debug.hpp>
#include <CRSLib/interrupt_lock.hpp>

#include "config.hpp"
#include "can_circular_queue.hpp"
#include "offset_id_impl_injector.hpp"
#include "offset_id.hpp"
#include "tx_unit.hpp"

namespace CRSLib::Can
{
	namespace Implement::TransmitterImp
	{
		template<auto index>
		struct ForIter
		{
			bool is_breaked{false};
		};
	}

	template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnum>
	class Transmitter final
	{
		std::tuple<TxUnit<OffsetIdsEnum> * ...> tx_units{};

		// 引数に渡した順に優先的に送信される(idの大小を反映できなかった)ので注意.
		template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnum>
		friend constexpr Transmitter make_transmitter(TxUnit<OffsetIdsEnum>& ... arg_tx_units) noexcept
		{
			return Transmitter{arg_tx_units};
		}

		constexpr Transmitter(TxUnit<OffsetIdsEnum>& ... arg_tx_units) noexcept:
			tx_units{&arg_tx_units ...}
		{}

	public:
		void push_tx_fifo() noexcept
		{
			using Implement::TransmitterImp;
			using namespace IntegerLiterals;

			// for文って偉大だね...
			auto for_body_par_tx_unit = [this]<size_t index>(ForIter<index>) constexpr noexcept
			{
				ForIter<index + 1> ret{};
				if constexpr(index == std::tuple_size_v<tx_units>) ret.is_breaked = true;

				const auto * tx_unit_p = std::get<index>(tx_units);
				using Enum = decltype(*tx_unit_p)::OffsetIdsEnum;

				auto for_body_par_offset_id = [tx_unit_p]<decltype(*tx_unit_p)::OffsetIdsEnum offset_id>(ForIter<offset_id>) constexpr noexcept
				{
					ForIter<static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(offset_id) + 1)> ret{};
					if constexpr(offset_id == Enum::n) ret.is_breaked = true;

					InterruptLock lock{};

					while(true)
					{
						if(HAL_CAN_GetTxMailboxesFreeLevel(Config::can_instance<can_x>) == 0) break;
						if(const auto opt_data_field = tx_unit_p.pop<offset_id>(); !opt_data_field) break;
						else
						{
							auto tx_header = tx_unit_p->make_default_can_tx_header<can_x, offset_id>();
							u32 mail_box{};
							if(HAL_CAN_AddTxMessage(Config::can_instance<can_x>, &tx_header, opt_data_field->data(), &mail_box) != HAL_OK)
							{
								Debug::set_error("Fail to call HAL_CAN_AddTxMessage.");
								Error_Handler();
								ret.is_breaked = true;
								return ret;
							}
						}
					}
				};

				compile_for(for_body_par_offset_id, ForIter<static_cast<Enum>(0)>{});
				
				return ret;
			};

			compile_for(for_body_par_tx_unit, ForIter<0_size_t>{});
		}
	};
}