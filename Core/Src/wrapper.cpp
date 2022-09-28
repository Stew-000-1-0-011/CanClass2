#include <can_sample.hpp>
#include <CRSLib/executor.hpp>

namespace Chibarobo2022
{
	CRSLib::Executor<void () noexcept, 100> executor{};
}

extern "C"
{
	inline void wrapper_cpp() noexcept
	{
		using namespace Chibarobo2022;
		while(true)
		{
			receiver.receive(executor);
			transmitter.transmit();
		}
	}
}