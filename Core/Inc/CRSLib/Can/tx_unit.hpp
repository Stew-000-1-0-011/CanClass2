#pragma once

#include "main.h"

#include <CRSLib/std_int.hpp>

#include "utility.hpp"
#include "unit_base.hpp"


namespace CRSLib::Can
{
	template<CanX can_x>
	inline constexpr CAN_TxHeaderTypeDef make_default_can_tx_header(const u32 id, bool is_rtr = false) noexcept
	{
		if(is_in_std_id_range(id))
		{
			return {.StdId = id, .ExtId = null_id, .IDE = false, .RTR = is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .TransmitGlobalTime = hcan<can_x>->Init.TimeTriggeredMode};
		}
		else
		{
			return {.StdId = ReservedCanId::null, .ExtId = id, .IDE = true, .RTR = is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA, .TransmitGlobalTime = hcan<can_x>->Init.TimeTriggeredMode};
		}
	}

	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class TxUnit final : public UnitBase<OffsetIdsEnum_>
	{
	public:
		TxUnit(const u32 base_id) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id}
		{}

	public:
		template<OffsetIdsEnum offset>
		void add_tx(const char(&buffer)[can_mtu]) const noexcept
		{
			std::get<static_cast<std::underlying_type_t<OffsetIdsEnum>>(offset)>(offset_ids).queue.push(buffer);
		}

		
	};
}