// 仮想関数と動的確保とfor文の組み合わせと、関数テンプレートと非動的確保とcompile_forの組み合わせ、どっちが良かっただろうか。

#pragma once

#include <utility>
#include <tuple>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>
#include <CRSLib/debug.hpp>
#include <CRSLib/executor.hpp>

#include "utility.hpp"
#include "offset_id.hpp"
#include "rx_id_impl_injector.hpp"
#include "rx_id.hpp"
#include "unit_base.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can::Implement
{
	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class RxUnit final : public UnitBase<OffsetIdsEnum_>
	{
		RxIdTuple<OffsetIdsEnum::n> rx_ids{};

	public:
		RxUnit(const u32 base_id) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id}
		{}

	public:
		template<size_t queue_size>
		void receive(const RxFrame& rx_frame, Executor<void () noexcept, queue_size>& executor) noexcept
		{
			auto for_body_par_offset_id = [this]<std::underlying_type_t<OffsetIdsEnum> offset_id>(CompileForIndex<offset_id>) noexcept
			{
				CompileForIndex<offset_id + 1> ret{};
				if constexpr(offset_id == to_underlying(OffsetIdsEnum::n))
				{
					ret.is_breaked = true;
					return ret;
				}

				if(rx_frame.header.id == base_id + offset_id)
				{
					std::get<offset_id>(rx_ids).queue.push(rx_frame);
					executor.queue.push(&std::get<offset_id>(rx_ids));
					ret.is_breaked = true;
					return ret;
				}

				return ret;
			}

			compile_for(for_body_par_offset_id, CompileForIndex<to_underlying<OffsetIdsEnum>(0)>{});
		}
	};
}
