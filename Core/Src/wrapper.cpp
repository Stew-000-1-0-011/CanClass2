#include <CRSLib/executor.hpp>
#include <CRSLib/Can/can_manager.hpp>

#include <can_sample.hpp>

namespace Chibarobo2022
{
	CRSLib::Executor<void () noexcept, 100> executor{};
}

extern "C"
{
	void wrapper_cpp() noexcept
	{

		using namespace Chibarobo2022;
		
		CRSLib::Can::CanManager<CRSLib::Can::CanX::single_can>::initialize();

		while(true)
		{
			receiver.receive<CRSLib::Can::CanX::single_can, CRSLib::Can::FifoIndex::fifo0>(executor);
			receiver.receive<CRSLib::Can::CanX::single_can, CRSLib::Can::FifoIndex::fifo1>(executor);
			transmitter.transmit();
		}
	}
}
