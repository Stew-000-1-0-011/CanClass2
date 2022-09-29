#pragma once

#include <concepts>
#include <bit>

namespace CRSLib::Can::Implement
{
	template<class T>
	concept IdImplInjectorBase = requires
	{
		T::queue_size;
	};
}
