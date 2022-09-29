#pragma once

#include "cmsis_for_cpp.h"

namespace CRSLib
{
	class InterruptDisabler final
	{
	public:
		InterruptDisabler() noexcept
		{
			stew_disable_irq();
		}

		InterruptDisabler(const InterruptDisabler&) = delete;
		InterruptDisabler(InterruptDisabler&&) = delete;
		InterruptDisabler& operator =(const InterruptDisabler&) = delete;
		InterruptDisabler& operator =(InterruptDisabler&&) = delete;

		~InterruptDisabler()
		{
			stew_enable_irq();
		}
	};
}
