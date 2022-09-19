#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/interrupt_safe_circular_queue.hpp>

#include "tx_id_impl_injector.hpp"
#include "utility.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	struct TxId final
	{
		using Impl = TxIdImplInjectorAdaptor<offset_id>;
		InterruptSafeCircularQueue<DataField, Impl::queue_size> queue{};
	};

	namespace Implement::TxIdImp
	{
		template<IsOffsetIdsEnum OffsetIdsEnum, OffsetIdsEnum ... offset_ids>
		std::tuple<TxId<offset_ids>...> calc_type_of_tx_id_tuple(std::integer_sequence<OffsetIdsEnum, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using TxIdTuple = decltype(Implement::OffsetIdImp::calc_type_of_tx_id_tuple(std::make_integer_sequence<decltype(n), n>));
}