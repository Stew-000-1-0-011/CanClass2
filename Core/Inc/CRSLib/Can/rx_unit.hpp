// 仮想関数と動的確保とfor文の組み合わせと、関数テンプレートと非動的確保とcompile_forの組み合わせ、どっちが良かっただろうか。

#pragma once

#include <utility>
#include <tuple>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>
#include <CRSLib/debug.hpp>

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
		FifoIndex fifo_index;
		RxIdTuple<OffsetIdsEnum::n> rx_ids{};

	public:
		RxUnit(const u32 base_id, const FifoIndex fifo_index) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id},
			fifo_index{fifo_index}
		{}

	public:
		// 受け取ったrx_frameを受け取るidがあれば, そのidのキューに格納する. キューに格納したらtrue, しなかったらfalseを返す.
		bool push_queue(const RxFrame& rx_frame) noexcept
		{
			bool is_pushed{false};

			auto for_body_par_offset_id = [this, &is_pushed]<std::underlying_type_t<OffsetIdsEnum> offset_id>(CompileForIndex<offset_id>) noexcept
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
					is_pushed = true;
					ret.is_breaked = true;
					return ret;
				}

				return ret;
			}

			compile_for(for_body_par_offset_id, CompileForIndex<to_underlying<OffsetIdsEnum>(0)>{});
			return is_pushed;
		}

		// 一つ以上フレームを格納しているrx_idがあれば, そのコールバックを1つ呼び出す.
		// コールバックを1つ呼び出せばtrueを、1つも呼び出さなければfalseを返す.
		bool call_once() noexcept
		{
			bool is_called{false};

			auto for_body_par_offset_id = [this, &is_called]<std::underlying_type_t<OffsetIdsEnum> offset_id>(CompileForIndex<offset_id>) noexcept
			{
				CompileForIndex<offset_id + 1> ret{};
				if constexpr(offset_id == to_underlying(OffsetIdsEnum::n))
				{
					ret.is_breaked = true;
					return ret;
				}

				if(std::get<offset_id>(rx_ids).call_once())
				{
					is_called = true;
					ret.is_breaked = true;
					return ret;
				}

				return ret;
			};

			compile_for(for_body_par_offset_id, CompileForIndex<to_underlying<OffsetIdsEnum>(0)>{});
			return is_called;
		}

		FilterId get_filter_id() const noexcept
		{
			return {.base_id = base_id, .align = align, .fifo_index = fifo_index};
		}
	};
}
