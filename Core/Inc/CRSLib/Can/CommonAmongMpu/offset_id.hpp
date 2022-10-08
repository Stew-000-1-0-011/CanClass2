#pragma once

#include <type_traits>
#include <concepts>

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>

namespace CRSLib::Can
{
	// 制約に落とし込められてないが, nが最大である必要がある。
	// また, 連番である必要がある。
	template<class T>
	concept OffsetIdsEnumC = std::is_enum_v<T> && std::same_as<std::underlying_type_t<T>, u32> && requires{T::n;} && (to_underlying(T::n) > 0);
}
