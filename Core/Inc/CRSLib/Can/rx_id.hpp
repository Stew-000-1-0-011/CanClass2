#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/interrupt_safe_circular_queue.hpp>
#include <CRSLib/executor.hpp>

#include "utility.hpp"
#include "rx_id_impl_injector.hpp"

namespace CRSLib::Can::Implement
{
	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>>
	struct RxId
	{
		static_assert([]{return true;}(), "You may write wrong callback.");
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>> && (!HasAnyCallback<RxIdImplInjector<offset_id>>)
	struct RxId<offset_id> final
	{
		using Impl = RxIdImplAdaptor<offset_id>;
		InterruptSafeCircularQueue<RxFrame, Impl::queue_size> queue{};

		RxId() = default;
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>> && HasMemberCallback<RxIdImplInjector<offset_id>>
	struct RxId<offset_id> final : Executable<void () noexcept>
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

		void execute() noexcept override
		{
			call_once();
		}
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsRxIdImplInjector<RxIdImplInjector<offset_id>> && HasNonMemberCallback<RxIdImplInjector<offset_id>>
	struct RxId<offset_id> final : Executable<void () noexcept>
	{
		using Impl = RxIdImplAdaptor<offset_id>;
		InterruptSafeCircularQueue<RxFrame, Impl::queue_size> queue{};

		bool call_once() noexcept override
		{
			if(const auto opt_rx_frame = queue.pop(); opt_rx_frame)
			{
				Impl::callback(*opt_rx_frame);
				return true;
			}
			else return false;
		}

		void execute() noexcept override
		{
			call_once();
		}
	};

	namespace RxIdImp
	{
		template<IsOffsetIdsEnum OffsetIdsEnum, std::underlying_type_t<OffsetIdsEnum> ... offset_ids>
		std::tuple<RxId<static_cast<OffsetIdsEnum>(offset_ids)>...> calc_type_of_rx_id_tuple(std::integer_sequence<std::underlying_type_t<OffsetIdsEnum>, offset_ids ...>);
	}

	template<IsOffsetIdsEnum auto n>
	using RxIdTuple = decltype(Implement::RxIdImp::calc_type_of_rx_id_tuple<decltype(n)>(std::make_integer_sequence<std::underlying_type_t<decltype(n)>, to_underlying(n)>()));
}
