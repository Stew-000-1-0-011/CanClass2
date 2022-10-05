#pragma once

#include "cmsis_for_cpp.h"

namespace CRSLib
{
	// これを使うような処理はごくごく短いものにとどめること.
	// 安易に手を出すな.(自戒)
	class InterruptDisabler final
	{
	public:
		InterruptDisabler() noexcept
		{
			stew_disable_irq();
		}

		InterruptDisabler(const InterruptDisabler&) = delete;
		InterruptDisabler(InterruptDisabler&&) = delete;
		InterruptDisabler& operator=(const InterruptDisabler&) = delete;
		InterruptDisabler& operator=(InterruptDisabler&&) = delete;

		~InterruptDisabler()
		{
			stew_enable_irq();
		}
	};
}
