#include <CRSLib/executor.hpp>
#include <CRSLib/Can/can_manager.hpp>
#include <CRSLib/Can/filter_manager.hpp>

#include <can_sample.hpp>

namespace Chibarobo2022
{
	CRSLib::Executor<void () noexcept, 100> executor{};
}

using namespace CRSLib;
using namespace Chibarobo2022;


extern "C"
{
	void wrapper_cpp() noexcept
	{

		
		Can::CanManager<CRSLib::Can::CanX::single_can>::initialize();

		Can::FilterManager::dynamic_initialize();
		Can::FilterManager::register_filter(Can::FilterManager::FilterId{.base_id = 0x20, .align = })


		while(true)
		{
			receiver.receive<CRSLib::Can::CanX::single_can, CRSLib::Can::FifoIndex::fifo0>(executor);
			receiver.receive<CRSLib::Can::CanX::single_can, CRSLib::Can::FifoIndex::fifo1>(executor);
			transmitter.transmit();
		}
	}
}
