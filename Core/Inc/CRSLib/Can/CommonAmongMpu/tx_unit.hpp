#pragma once

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>

#include "utility.hpp"
#include "config.hpp"
#include "tx_id.hpp"
#include "unit_base.hpp"
#include "abstract_mpu_specific_constraint_check.hpp"

namespace CRSLib::Can::Implement
{
	struct TxUnitInterface
	{
		virtual void transmit() noexcept = 0;
		virtual u32 get_base_id() const noexcept = 0;
	};

	template<OffsetIdsEnumC OffsetIdsEnum_, TxFrameC TxFrame>
	class TxUnit final : public UnitBase<OffsetIdsEnum_>, TxUnitInterface
	{
		using OffsetIdsEnum = OffsetIdsEnum_;
		TxIdTuple<OffsetIdsEnum::n, TxFrame> tx_ids{};

	public:
		TxUnit(const u32 base_id) noexcept:
			UnitBase<OffsetIdsEnum>{base_id}
		{}

		void transmit(Pillarbox& pillarbox) noexcept override
		{
			auto for_body_par_id = [this]<std::underlying_type_t<OffsetIdsEnum> offset_id, std::underlying_type_t<OffsetIdsEnum> n>(CompileForIndex<offset_id, n>) noexcept
			{
				CompileForIndex<offset_id + 1, n> ret{};

				ret.is_breaked = std::get<offset_id>(tx_ids).transmit(pillarbox, this->base_id);
				return ret;
			};

			compile_for(for_body_par_id, CompileForIndex<static_cast<std::underlying_type_t<OffsetIdsEnum>>(0), to_underlying(OffsetIdsEnum::n)>{});
		}

		u32 get_base_id() const noexcept
		{
			return base_id;
		}

		void set_base_id(const u32 base_id) noexcept
		{
			this->base_id = base_id;
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
