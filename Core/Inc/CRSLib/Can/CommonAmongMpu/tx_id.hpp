#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#include <CRSLib/circular_queue.hpp>

#include "abstract_mpu_specific_constraint_check.hpp"
#include "tx_id_impl_injector.hpp"
#include "utility.hpp"
#include "handle.hpp"

namespace CRSLib::Can
{
	template<OffsetIdsEnumC auto offset_id>
	struct TxId final
	{
		using Impl = TxIdImplInjectorAdaptor<offset_id>;
		SafeCircularQueue<TxFrame, Impl::queue_size> queue{};

		// Mailboxが満杯になった(あるいはエラーが発生した)らfalse, そうでなければtrueを返す.
		bool transmit(Pillarbox& pillarbox, const u32 base_id) noexcept
		{
			while(true)
			{
				if(!pillarbox.empty())
				{
					return false;
				}
				else
				{
					if(auto opt_tx_frame = queue.pop(); !opt_tx_frame)
					{
						return true;
					}
					else
					{
						pillarbox.post(base_id, *opt_tx_frame);
					}
				}
			}
		}
	};

	namespace Implement::TxIdImp
	{
		template<OffsetIdsEnumC OffsetIdsEnum, std::underlying_type_t<OffsetIdsEnum> ... offset_ids>
		std::tuple<TxId<static_cast<OffsetIdsEnum>(offset_ids)>...> calc_type_of_tx_id_tuple(std::integer_sequence<std::underlying_type_t<OffsetIdsEnum>, offset_ids ...>);
	}

	template<OffsetIdsEnumC auto n>
	using TxIdTuple = decltype(Implement::TxIdImp::calc_type_of_tx_id_tuple<decltype(n)>(std::make_integer_sequence<std::underlying_type_t<decltype(n)>, to_underlying(n)>()));
}
