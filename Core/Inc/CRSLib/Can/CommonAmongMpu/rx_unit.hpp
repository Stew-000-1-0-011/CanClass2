// 仮想関数と動的確保とfor文の組み合わせと、関数テンプレートと非動的確保とcompile_forの組み合わせ、どっちが良かっただろうか。

#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>
#include <CRSLib/debug.hpp>
#include <CRSLib/executor.hpp>
#include <CRSLib/compile_for.hpp>

#include "utility.hpp"
#include "offset_id.hpp"
#include "rx_id_impl_injector.hpp"
#include "rx_id.hpp"
#include "unit_base.hpp"

namespace CRSLib::Can::Implement
{
	template<OffsetIdsEnumC OffsetIdsEnum_>
	class RxUnit final : public UnitBase
	{
	public:
		using OffsetIdsEnum = OffsetIdsEnum_;

	private:
		RxIdTuple<OffsetIdsEnum::n> rx_ids;

	public:
		constexpr RxUnit(const u32 base_id, const auto ... args) noexcept:
			UnitBase{to_underlying(OffsetIdsEnum::n), base_id},
			rx_ids{args ...}
		{}

	public:
		template<size_t queue_size>
		void receive(const RxFrame& rx_frame, Executor<void () noexcept, queue_size>& executor) noexcept
		{
			auto for_body_par_offset_id = [this, &rx_frame, &executor]
			<std::underlying_type_t<OffsetIdsEnum> offset_id, std::underlying_type_t<OffsetIdsEnum> n>
			(CompileForIndex<offset_id, n>) noexcept
			{
				CompileForIndex<offset_id + 1, n> ret{};

				if(rx_frame.header.get_id() == this->base_id + offset_id)
				{
					std::get<offset_id>(rx_ids).queue.push(rx_frame);

					if constexpr(HasAnyCallback<RxIdImplInjector<static_cast<OffsetIdsEnum>(offset_id)>>)
					{
						executor.queue.push(&std::get<offset_id>(rx_ids));
					}

					ret.is_breaked = true;
					return ret;
				}

				return ret;
			};

			compile_for(for_body_par_offset_id, CompileForIndex<static_cast<std::underlying_type_t<OffsetIdsEnum>>(0), to_underlying(OffsetIdsEnum::n)>{});
		}
	};
}
