#pragma once

namespace CRSLib::Can::STM32f1
{
	enum class CanX
	{
		single_can,
		can1,
		can2
	};

	enum class FifoIndex : u32
	{
		fifo0 = CAN_RX_FIFO0,
		fifo1 = CAN_RX_FIFO1
	};

	// 定義してね
    inline constexpr i_ptr can_instance(CanX can_x) noexcept
    {
        switch(can_x)
        {
        case CanX::single_can:
        case CanX::can1:
            return static_cast<i_ptr>(0x4000'6400);
        case CanX::can2:
            return static_cast<i_ptr>(0x4000'6800);
        }
    }
}