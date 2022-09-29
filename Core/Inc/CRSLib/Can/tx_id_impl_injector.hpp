#pragma once

#include <main.h>

#include <CRSLib/std_int.hpp>
#include "utility.hpp"
#include "offset_id.hpp"
#include "id_impl_injector_base.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	struct TxIdImplInjector;

	namespace Implement::TxIdImplInjectorImp
	{
		template<class T>
		inline constexpr bool is_tx_id_impl_injector = false;

		template<auto offset_id>
		inline constexpr bool is_tx_id_impl_injector<TxIdImplInjector<offset_id>> = true;
	}

	template<class T>
	concept IsTxIdImplInjector = Implement::TxIdImplInjectorImp::is_tx_id_impl_injector<T> && Implement::IdImplInjectorBase<T>;

	template<IsOffsetIdsEnum auto offset_id>
	requires IsTxIdImplInjector<TxIdImplInjector<offset_id>>
	using TxIdImplInjectorAdaptor = TxIdImplInjector<offset_id>;

//	idはいらない(送信部ではわかっているため)
	struct TxHeader final
	{
		u8 dlc{8};
		bool is_rtr{false};
	};

	template<CanX can_x>
	inline constexpr CAN_TxHeaderTypeDef make_tx_header(const u32 id, const TxHeader& tx_header) noexcept
	{
		using namespace CRSLib::IntegerLiterals;

		if(is_in_std_id_range(id))
		{
			return CAN_TxHeaderTypeDef{.StdId = id, .ExtId = null_id, .IDE = false, .RTR = tx_header.is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = tx_header.is_rtr ? 0_u32 : tx_header.dlc, .TransmitGlobalTime = Implement::hcan<can_x>.Init.TimeTriggeredMode};
		}
		else
		{
			return CAN_TxHeaderTypeDef{.StdId = null_id, .ExtId = id, .IDE = true, .RTR = tx_header.is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = tx_header.is_rtr ? 0_u32 : tx_header.dlc, .TransmitGlobalTime = Implement::hcan<can_x>.Init.TimeTriggeredMode};
		}
	}

	struct TxFrame final
	{
		DataField data;
		TxHeader header;
	};
}
