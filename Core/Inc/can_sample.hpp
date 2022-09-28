#pragma once

#include <CRSLib/std_int.hpp>

#include <CRSLib/Can/utility.hpp>
#include <CRSLib/Can/transmitter.hpp>
#include <CRSLib/Can/receiver.hpp>

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


template<>
struct CRSLib::Can::TxIdImplInjector<Chibarobo2022::MyMotorInfo::Info>
{
	static constexpr size_t queue_size = 1;
};

namespace Chibarobo2022
{
	inline CRSLib::Can::Transmitter<CRSLib::Can::CanX::single_can, Chibarobo2022::MyMotorInfo> transmitter{};
}

template<>
struct CRSLib::Can::RxIdImplInjector<Chibarobo2022::MyMotorCommand::Command>
{
	static constexpr size_t queue_size = 10;
};

template<>
struct CRSLib::Can::RxIdImplInjector<Chibarobo2022::MyMotorCommand::Target>
{
	static constexpr size_t queue_size = 10;

	void callback(const CRSLib::Can::RxFrame& rx_frame) noexcept
	{
		CRSLib::Can::DataField data_field = rx_frame.data;
		Chibarobo2022::transmitter.template get_tx_unit<0>().template push<Chibarobo2022::MyMotorInfo::Info>(data_field);
	}
};

template<>
struct CRSLib::Can::RxIdImplInjector<Chibarobo2022::MyMotorCommand::SubCommand>
{
	static constexpr size_t queue_size = 1;

	void callback(const CRSLib::Can::RxFrame& rx_frame) noexcept
	{
		CRSLib::Can::DataField data_field = rx_frame.data;
		Chibarobo2022::transmitter.template get_tx_unit<0>().template push<Chibarobo2022::MyMotorInfo::Info>(data_field);
	}
};

namespace Chibarobo2022
{
	inline CRSLib::Can::Receiver<Chibarobo2022::MyMotorCommand> receiver{};
}