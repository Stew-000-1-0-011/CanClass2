// OffsetId

#pragma once

#include <type_traits>
#include <concepts>
#include <bit>

#include <CRSLib/std_int.hpp>
#include "utility.hpp"

namespace CRSLib::Can
{
	// 制約に落とし込められてないが、nが最大である必要がある。
	// また、連番である必要がある。
	template<class T>
	concept IsOffsetIdsEnum = std::is_enum_v<T> && std::same_as<std::underlying_type_t<T>, u32> && requires{T::n;} && (static_cast<std::underlying_type_t<T>>(T::n) > 0);

	template<IsOffsetIdsEnum auto offset_id>
	struct OffsetIdImplInjector;

	namespace Implement::OffsetIdImplInjectorImp
	{
		template<class T>
		struct IsOffsetIdImplInjectorTraits : std::false_type{};

		template<auto can_offset_id>
		struct IsOffsetIdImplInjectorTraits<OffsetIdImplInjector<can_offset_id>> : std::true_type{};
	}

	using RxUnitCallback = void (const DataField&) noexcept;

	template<class T>
	concept has_callback = requires
	{
		std::same_as<decltype(T::callback), RxUnitCallback>;
	};

	template<class T>
	concept IsOffsetIdImplInjector = Implement::OffsetIdImplInjectorImp::IsOffsetIdImplInjectorTraits<T>::value && requires
	{
		T::endian;
		requires std::same_as<decltype(T::endian), std::endian>;

		T::dlc;
		requires std::same_as<decltype(T::dlc), u32>;

		T::queue_size;
		requires std::same_as<decltype(T::queue_size), size_t>;

		requires has_callback<T> || !requires{T::callback;};
	};

	template<IsOffsetIdsEnum auto offset_id>
	requires IsOffsetIdImplInjector<OffsetIdImplInjector<offset_id>>
	using OffsetIdImplInjectorAdaptor = OffsetIdImplInjector<offset_id>;
}
