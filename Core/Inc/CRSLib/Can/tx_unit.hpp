#pragma once

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>

#include "utility.hpp"
#include "config.hpp"
#include "tx_id_impl_injector.hpp"
#include "unit_base.hpp"


namespace CRSLib::Can
{
	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class TxUnit final : public UnitBase<OffsetIdsEnum_>
	{
		TxIdTuple<OffsetIdsEnum::n> tx_ids{};

	public:
		TxUnit(const u32 base_id) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id}
		{}
	
		template<CanX can_x, OffsetIdsEnum auto offset_id>
		inline constexpr CAN_TxHeaderTypeDef make_default_tx_header(bool is_remote = false) noexcept
		{
			const u32 id = base_id + offset_id;
			if(is_in_std_id_range(id))
			{
				return {.StdId = id, .ExtId = null_id, .IDE = false, .RTR = is_remote ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = is_remote ? 0 : OffsetIdImplInjectorAdaptor<offset_id>::dlc, .TransmitGlobalTime = &hcan<can_x>->Init.TimeTriggeredMode};
			}
			else
			{
				return {.StdId = null_id, .ExtId = id, .IDE = true, .RTR = is_remote ? CAN_RTR_REMOTE : CAN_RTR_DATA, .DLC = is_remote ? 0 : OffsetIdImplInjectorAdaptor<offset_id>::dlc, .TransmitGlobalTime = &hcan<can_x>->Init.TimeTriggeredMode};
			}
		}

		template<CanX can_x>
		void transmit() noexcept
		{
			InterruptDisabler disabler{};

			auto for_body_par_id = [this]<std::underlying_type_t<OffsetIdsEnum> offset_id>(CompileForIndex<offset_id>) noexcept
			{
				CompileForIndex<offset_id + 1> ret{};
				if constexpr(offset_id == to_underlying(OffsetIdsEnum::n))
				{
					ret.is_breaked = true;
					return ret;
				}

				ret.is_breaked = std::get<offset_id>(tx_ids).transmit(make_default_tx_header<can_x, static_cast<OffsetIdsEnum>(offset_id)>());
				return ret;
			};

			compile_for(for_body_par_id, CompileForIndex<to_underlying<OffsetIdsEnum>(0)>{});
		}

		template<OffsetIdsEnum auto offset_id>
		void push(const DataField& data_field) noexcept
		{
			std::get<to_underlying(offset_id)>(offset_ids).queue.push(data_field);
		}

		template<OffsetIdsEnum auto offset_id>
		void clear() noexcept
		{
			std::get<to_underlying(offset_id)>(offset_ids).queue.clear();
		}
	};
}