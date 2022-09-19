#pragma once

#include <type_traits>

namespace CRSLib
{
	template<class Enum>
	requires std::is_enum_v<Enum>
	constexpr auto to_underlying(const Enum x) noexcept
	{
		return static_cast<std::underlying_type_t<Enum>>(x);
	}
}