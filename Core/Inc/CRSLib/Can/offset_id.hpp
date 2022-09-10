#pragma once

#include <array>
#include <utility>
#include <tuple>

#include "can_circular_queue.hpp"
#include "offset_id_impl_injector.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	class OffsetId final
	{
		public:
		using Impl = OffsetIdImplInjectorAdaptor<offset_id>;
		CanCircularQueue<Impl::queue_size> queue;
	};

	namespace Implement::OffsetIdImp
	{
		template<IsOffsetIdsEnum OffsetIdsEnum, OffsetIdsEnum ... offset_ids>
		std::tuple<OffsetId<offset_ids>...> calc_type_of_offset_id_tuple(std::integer_sequence<OffsetIdsEnum, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using OffsetIdTuple = decltype(Implement::OffsetIdImp::calc_type_of_offset_id_tuple(std::make_integer_sequence<decltype(n), n>));
}