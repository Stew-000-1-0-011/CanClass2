#pragma once

#include <CRSLib/std_int.hpp>

#include <CRSLib/Can/CommonAmongMpu/utility.hpp>
#include <CRSLib/Can/CommonAmongMpu/transmitter.hpp>
#include <CRSLib/Can/CommonAmongMpu/receiver.hpp>

namespace Chibarobo2022
{
	using namespace CRSLib::IntegerTypes;
	// 
	enum class MyMotorCommand : u32
	{
		Command,
		Target,
		SubCommand,

		n
	};

	enum class MyMotorInfo : u32
	{
		Info,

		n
	};
}

namespace CRSLib::Can
{
	template<>
	struct TxIdImplInjector<Chibarobo2022::MyMotorInfo::Info>
	{
		static constexpr size_t queue_size() noexcept{return 1;}
	};

	template<>
	struct RxIdImplInjector<Chibarobo2022::MyMotorCommand::Command>
	{
		static constexpr size_t queue_size() noexcept{return 10;}
	};
}

namespace Chibarobo2022
{
	inline CRSLib::Can::Transmitter<Chibarobo2022::MyMotorInfo> transmitter{(u32)0x40};
}

namespace CRSLib::Can
{
	template<>
	struct RxIdImplInjector<Chibarobo2022::MyMotorCommand::Target>
	{
		static constexpr size_t queue_size() noexcept{return 10;}

		void callback(const CRSLib::Can::RxFrame& rx_frame) noexcept
		{
			Chibarobo2022::transmitter.template push<0, Chibarobo2022::MyMotorInfo::Info>(TxFrame{{rx_frame.header.dlc}, rx_frame.data});
		}
	};

	template<>
	struct RxIdImplInjector<Chibarobo2022::MyMotorCommand::SubCommand>
	{
		static constexpr size_t queue_size() noexcept{return 1;}

		void callback(const CRSLib::Can::RxFrame& rx_frame) noexcept
		{
			Chibarobo2022::transmitter.template push<0, Chibarobo2022::MyMotorInfo::Info>(TxFrame{{rx_frame.header.dlc}, rx_frame.data});
		}
	};

}

namespace Chibarobo2022
{
	inline CRSLib::Can::Receiver<Chibarobo2022::MyMotorCommand> receiver{(u32)0x41};
}
