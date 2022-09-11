#pragma once

#include <array>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/debug.hpp>

#include "utility.hpp"
#include "handle.hpp"
#include "offset_id_impl_injector.hpp"
#include "offset_id.hpp"
#include "unit_base.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum OffsetIdsEnum_>
	class RxUnit final : public UnitBase<OffsetIdsEnum_>
	{
	private:
		static constexpr std::array<RxUnitCallback *, id_num> callbacks
		{
			[]<u32 ... indices>
			(std::integer_sequence<u32, indices ...>) noexcept
			{
				template<u32 index>
				using Impl = OffsetIdImplInjectorAdaptor<static_cast<OffsetIdsEnum>(index)>;

				std::array<RxUnitCallback *, id_num> ret{};
				((ret[indices] = (has_callback<Impl<indices>> ? Impl<indices>::callback : nullptr)), ...);
				return ret;
			}
			(std::make_integer_sequence<u32, id_num>())
		};

		FifoIndex fifo_index;

	public:
		RxUnit(const u32 base_id, const FifoIndex fifo_index) noexcept:
			UnitBase<OffsetIdsEnum_>{base_id},
			fifo_index{fifo_index}
		{}

	public:
		void call(const u32 id, const DataField& buffer) const noexcept
		{
			const u32 offset_id = id - base_id;
			callbacks[offset_id](buffer);
		}

		FilterId get_filter_id() const noexcept
		{
			return {.base_id = base_id, .align = align, .fifo_index = fifo_index};
		}
	};
}
