#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#include <CRSLib/interrupt_safe_circular_queue.hpp>

#include "tx_id_impl_injector.hpp"
#include "utility.hpp"
#include "handle.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	struct TxId final
	{
		using Impl = TxIdImplInjectorAdaptor<offset_id>;
		InterruptSafeCircularQueue<TxFrame, Impl::queue_size> queue{};

		// Mailboxが満杯になった(あるいはエラーが発生した)らfalse, そうでなければtrueを返す.
		template<CanX can_x>
		bool transmit(const u32 base_id) noexcept
		{
			while(true)
			{
				if(HAL_CAN_GetTxMailboxesFreeLevel(&Implement::hcan<can_x>) == 0) return false;
				if(auto opt_tx_frame = queue.pop(); !opt_tx_frame)
				{
					return true;
				}
				else
				{

					auto tx_header = make_tx_header<can_x>(base_id + to_underlying(offset_id), opt_tx_frame->header);
					u32 mail_box{};
					if(HAL_CAN_AddTxMessage(&Implement::hcan<can_x>, &tx_header, opt_tx_frame->data.data(), &mail_box) != HAL_OK)
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
		template<IsOffsetIdsEnum OffsetIdsEnum, std::underlying_type_t<OffsetIdsEnum> ... offset_ids>
		std::tuple<TxId<static_cast<OffsetIdsEnum>(offset_ids)>...> calc_type_of_tx_id_tuple(std::integer_sequence<std::underlying_type_t<OffsetIdsEnum>, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using TxIdTuple = decltype(Implement::TxIdImp::calc_type_of_tx_id_tuple<decltype(n)>(std::make_integer_sequence<std::underlying_type_t<decltype(n)>, to_underlying(n)>()));
}
