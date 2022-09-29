// OffsetId

#pragma once

#include <CRSLib/std_int.hpp>
#include "utility.hpp"
#include "offset_id.hpp"
#include "id_impl_injector_base.hpp"

namespace CRSLib::Can
{
	struct RxHeader final
	{
		u32 id{null_id};
		u32 time_stamp{static_cast<u32>(-1)};
		bool is_remote{false};
		u8 dlc{0};
	};

	struct RxFrame final
	{
		RxHeader header{};
		DataField data{};
	};

	RxHeader make_rx_header(const CAN_RxHeaderTypeDef& rx_header) noexcept
	{
		return
			{
				rx_header.IDE == CAN_ID_STD ? rx_header.StdId : rx_header.ExtId,
				rx_header.Timestamp,
				rx_header.RTR == CAN_RTR_REMOTE,
				static_cast<u8>(rx_header.DLC)
			};
	}

	template<IsOffsetIdsEnum auto offset_id>
	class RxIdImplInjector;

	namespace Implement
	{
		template<class T>
		concept HasAnyCallback =
			requires(RxFrame rx_frame)
			{
				T::callback(rx_frame);
			} ||
			requires(RxFrame rx_frame, typename T::CallbackArg * arg)
			{
				T::callback(rx_frame, arg);
			};

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
