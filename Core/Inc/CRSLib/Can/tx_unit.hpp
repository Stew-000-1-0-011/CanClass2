#pragma once

#include "main.h"

#include <CRSLib/std_int.hpp>

#include "utility.hpp"
#include "offset_id_impl_injector.hpp"
#include "unit_base.hpp"


namespace CRSLib::Can
{
	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class TxUnit final : public UnitBase<OffsetIdsEnum_>
	{
	public:
		TxUnit(const u32 base_id) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id}
		{}
	
		template<CanX can_x, OffsetIdsEnum auto offset_id>
		inline constexpr CAN_TxHeaderTypeDef make_default_can_tx_header(bool is_rtr = false) noexcept
		{
			const u32 id = base_id + offset_id;
			if(is_in_std_id_range(id))
			{
				return {.StdId = id, .ExtId = null_id, .IDE = false, .RTR = is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = is_rtr ? 0 : OffsetIdImplInjectorAdaptor<offset_id>::dlc, .TransmitGlobalTime = hcan<can_x>->Init.TimeTriggeredMode};
			}
			else
			{
				return {.StdId = null_id, .ExtId = id, .IDE = true, .RTR = is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = is_rtr ? 0 : OffsetIdImplInjectorAdaptor<offset_id>::dlc, .TransmitGlobalTime = hcan<can_x>->Init.TimeTriggeredMode};
			}
		}
	};

	namespace Implement::TxUnitImp
	{
		template<class T>
		inline constexpr bool is_tx_unit = false;

		template<IsOffsetIdsEnum OffsetIdsEnum>
		inline constexpr bool is_tx_unit<TxUnit<OffsetIdsEnum>> = true;
	}

	template<class T>
	concept IsTxUnit = Implement::TxUnitImp::is_tx_unit<T>;
}