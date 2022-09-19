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

			while(true)
			{
				if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan<can_x>) == 0) break;
				if(const auto opt_data_field = tx_unit_p.pop<offset_id>(); !opt_data_field) break;
				else
				{
					auto tx_header = make_default_tx_header<can_x, offset_id>();
					u32 mail_box{};
					if(HAL_CAN_AddTxMessage(&hcan<can_x>, &tx_header, opt_data_field->data(), &mail_box) != HAL_OK)
					{
						Debug::set_error("Fail to call HAL_CAN_AddTxMessage.");
						Error_Handler();
						ret.is_breaked = true;
						return ret;
					}
				}
			}
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