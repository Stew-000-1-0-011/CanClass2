// OffsetId

#pragma once

#include <CRSLib/std_int.hpp>
#include "utility.hpp"
#include "offset_id.hpp"
#include "id_impl_injector_base.hpp"

namespace CRSLib::Can
{
	template<IsOffsetIdsEnum auto offset_id>
	class RxIdImplInjector;

	namespace Implement
	{
		template<class T>
		concept HasNonMemberCallback = requires(RxFrame rx_frame)
		{
			{T::callback(rx_frame)} noexcept;
		};

		template<class T>
		concept HasMemberCallback = requires(RxFrame rx_frame, typename T::CallbackArg * arg)
		{
			{T::callback(rx_frame, arg)} noexcept;
		};

		namespace RxIdImplInjectorImp
		{
			template<class T>
			inline constexpr bool is_rx_id_impl_injector = false;

			template<auto offset_id>
			inline constexpr bool is_rx_id_impl_injector<RxIdImplInjector<offset_id>> = true;
		}

		template<class T>
		concept IsRxIdImplInjector = RxIdImplInjectorImp::is_rx_id_impl_injector<T> && IdImplInjectorBase<T>;

		template<IsOffsetIdsEnum auto offset_id>
		requires IsRxIdImplInjector<RxIdImplInjector<offset_id>>
		using RxIdImplAdaptor = RxIdImplInjector<offset_id>;
	}
}
