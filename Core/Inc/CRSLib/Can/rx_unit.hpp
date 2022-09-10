// 割り込み対応、並列処理非対応
// 割り込みのみでシングルスレッドの場合、ある資源に対するアクセスは決して同時には発生しない。
// よってその資源に対するアクセスは、アクセス状態の読み取りと書き換えを続けて書くことで競合を防ぐことができる。

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
		void call(const u32 id, const char(& buffer)[can_mtu]) const noexcept
		{
			const u32 offset_id = id - base_id;
			callbacks[offset_id](buffer);
		}
		
		template<CanX can_x>
		void plug_ears() const noexcept
		{
			FilterManager::plug_ears<can_x>(base_id, fifo_index);
			is_active = false;
		}

		template<OffsetIdsEnum offset>
		bool receive(char (&buffer)[can_mtu]) const noexcept
		{
			return can_manager->receive(base_id + offset, buffer);
		}

		FifoIndex get_fifo_index() const noexcept
		{
			return fifo_index;
		}
	};
}
