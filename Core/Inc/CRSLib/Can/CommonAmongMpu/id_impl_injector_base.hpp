#pragma once

namespace CRSLib::Can::Implement
{
	template<class T>
	concept IdImplInjectorBase = requires
	{
		T::queue_size;
	};
}
