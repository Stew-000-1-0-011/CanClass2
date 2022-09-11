#pragma once

#include "cmsis_for_cpp.h"

namespace CRSLib
{
	class InterruptLock final
	{
		InterruptLock() noexcept
		{
			stew_disable_irq();
		}

		InterruptLock(const InterruptLock&) = delete;
		InterruptLock(InterruptLock&&) = delete;
		InterruptLock& operator =(const InterruptLock&) = delete;
		InterruptLock& operator =(InterruptLock&&) = delete;

		~InterruptLock()
		{
			stew_enable_irq();
		}
	};
}