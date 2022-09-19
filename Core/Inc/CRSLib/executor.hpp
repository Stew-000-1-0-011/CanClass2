#pragma once

#include "interrupt_safe_circular_queue.hpp"

namespace CRSLib
{
	template<class F, size_t queue_size>
	struct Executor final
	{
		static_assert([]{return false;}(), "Wrong template argument.");
	};

	template<size_t queue_size, class Ret, class ... Args>
	struct Executor<Ret (Args ...), queue_size> final
	{
		static_assert([]{return false;}(), "Function must be specified noexcept.");
	};

	template<size_t queue_size, class Ret, class ... Args>
	struct Executor<Ret (Args...) noexcept, queue_size> final
	{
		using FunctionType = Ret (Args ...);
	
		struct Interface
		{
			virtual Ret execute(Args ...) noexcept = 0;
			virtual ~Interface() = default;
		};
	
		InterruptSafeCircularQueue<FunctionType *, queue_size> queue{};

		void spin_once() noexcept
		{
			if(auto opt_executable = queue.pop(); opt_executable)
			{
				opt_executable->execute();
			}
		}
	};
}