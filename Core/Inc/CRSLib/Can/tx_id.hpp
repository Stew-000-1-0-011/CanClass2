#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/interrupt_safe_circular_queue.hpp>

#include "tx_id_impl_injector.hpp"
#include "utility.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	struct TxId final
	{
		using Impl = TxIdImplInjectorAdaptor<offset_id>;
		InterruptSafeCircularQueue<DataField, Impl::queue_size> queue{};

		// Mailboxが満杯になった(あるいはエラーが発生した)らfalse, そうでなければtrueを返す.
		bool transmit(const CAN_TxHeaderTypeDef& tx_header) noexcept
		{
			while(true)
			{
				if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan<can_x>) == 0) return false;
				if(const auto opt_data_field = tx_unit_p.pop<offset_id>(); !opt_data_field) return true;
				else
				{
					u32 mail_box{};
					if(HAL_CAN_AddTxMessage(&hcan<can_x>, &tx_header, opt_data_field->data(), &mail_box) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_AddTxMessage.");
						Error_Handler();
						return false;
					}
				}
			}
		}
	};

	namespace Implement::TxIdImp
	{
		template<IsOffsetIdsEnum OffsetIdsEnum, OffsetIdsEnum ... offset_ids>
		std::tuple<TxId<offset_ids>...> calc_type_of_tx_id_tuple(std::integer_sequence<OffsetIdsEnum, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using TxIdTuple = decltype(Implement::OffsetIdImp::calc_type_of_tx_id_tuple(std::make_integer_sequence<decltype(n), n>));
}