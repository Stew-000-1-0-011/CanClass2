#pragma once

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>

#include "utility.hpp"
#include "config.hpp"
#include "tx_id.hpp"
#include "unit_base.hpp"


namespace CRSLib::Can
{
	template<IsOffsetIdsEnum OffsetIdsEnum>
	inline constexpr u32 tx_unit_base_id = null_id;

	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class TxUnit final : public UnitBase<OffsetIdsEnum_>
	{
		using OffsetIdsEnum = OffsetIdsEnum_;
		TxIdTuple<OffsetIdsEnum::n> tx_ids{};

	public:
		TxUnit() noexcept:
			UnitBase<OffsetIdsEnum>{tx_unit_base_id<OffsetIdsEnum>}
		{}

		template<CanX can_x>
		void transmit() noexcept
		{
			InterruptDisabler disabler{};

			auto for_body_par_id = [this]<std::underlying_type_t<OffsetIdsEnum> offset_id, std::underlying_type_t<OffsetIdsEnum> n>(CompileForIndex<offset_id, n>) noexcept
			{
				CompileForIndex<offset_id + 1, n> ret{};

				ret.is_breaked = std::get<offset_id>(tx_ids).template transmit<can_x>(this->base_id);
				return ret;
			};

			compile_for(for_body_par_id, CompileForIndex<static_cast<std::underlying_type_t<OffsetIdsEnum>>(0), to_underlying(OffsetIdsEnum::n)>{});
		}

		template<OffsetIdsEnum offset_id>
		void push(const TxFrame& tx_frame) noexcept
		{
			std::get<to_underlying(offset_id)>(tx_ids).queue.push(tx_frame);
		}

		template<OffsetIdsEnum offset_id>
		void clear() noexcept
		{
			std::get<to_underlying(offset_id)>(tx_ids).queue.clear();
		}
	};
}
