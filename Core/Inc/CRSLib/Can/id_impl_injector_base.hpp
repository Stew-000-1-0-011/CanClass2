#pragma once

#include <concepts>
#include <bit>

namespace CRSLib::Can::Implement
{
	template<class T>
	concept IdImplInjectorBase = requires
	{
		T::queue_size;
		requires std::same_as<decltype(T::queue_size), size_t>;
	};
}