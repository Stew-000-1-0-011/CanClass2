#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/interrupt_safe_circular_queue.hpp>

#include "utility.hpp"
#include "rx_id_impl_injector.hpp"

namespace CRSLib::Can::Implement
{
	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>>
	struct RxId final : RxIdBase
	{
		using Impl = RxIdImplAdaptor<offset_id>;
		InterruptSafeCircularQueue<RxFrame, Impl::queue_size> queue{};

		RxId() = default;
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>> && HasMemberCallback<RxIdImplInjector<offset_id>>
	struct RxId final : RxIdBase
	{
		using Impl = RxIdImplAdaptor<offset_id>;
		InterruptSafeCircularQueue<RxFrame, Impl::queue_size> queue{};
		Impl::CallbackArg * arg;

		constexpr RxId(Impl::CallbackArg *const arg) noexcept:
			arg{arg}
		{}

		bool call_once() noexcept override
		{
			if(const auto opt_rx_frame = queue.pop(); opt_rx_frame)
			{
				Impl::callback(*opt_rx_frame, arg);
				return true;
			}
			else return false;
		}
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>> && HasNonMemberCallback<RxIdImplInjector<offset_id>>
	struct RxId final : RxIdBase
	{
		using Impl = RxIdImplAdaptor<offset_id>;
		InterruptSafeCircularQueue<RxFrame, Impl::queue_size> queue{};

		constexpr RxId(Impl::CallbackArg *const arg) noexcept:
			arg{arg}
		{}

		bool call_once() noexcept override
		{
			if(const auto opt_rx_frame = queue.pop(); opt_rx_frame)
			{
				Impl::callback(*opt_rx_frame);
				return true;
			}
			else return false;
		}
	};

	namespace RxIdImp
	{
		template<IsOffsetIdsEnum OffsetIdsEnum, OffsetIdsEnum ... offset_ids>
		std::tuple<RxId<offset_ids>...> calc_type_of_rx_id_tuple(std::integer_sequence<OffsetIdsEnum, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using RxIdTuple = decltype(Implement::OffsetIdImp::calc_type_of_rx_id_tuple(std::make_integer_sequence<decltype(n), n>));
}