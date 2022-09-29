#pragma once

#include <main.h>

#include "config.hpp"
#include "utility.hpp"

namespace CRSLib::Can::Implement
{
    template<CanX can_x>
    CAN_HandleTypeDef hcan
    {
        .Instance = Config::can_instance<can_x>,

        // ほぼCanClass2(Ryunika先輩作)からコピペしたもの.
        // Prescalerは多分4であってるんじゃないかな...ここだけ実行時に決定するように書かれてたので変更
        .Init =
        {
            .Prescaler = 4,
            .Mode = CAN_MODE_NORMAL,

            //Sample-Point at: (1+15)/(1+15+2)=88.9% where CANopen states "The location of the sample point must be as close as possible to 87,5 % of the bit time."
            .SyncJumpWidth = CAN_SJW_1TQ,
            .TimeSeg1 = CAN_BS1_15TQ,
            .TimeSeg2 = CAN_BS2_2TQ,

            .TimeTriggeredMode = DISABLE,
            .AutoBusOff = ENABLE,
            .AutoWakeUp = DISABLE,
            .AutoRetransmission = ENABLE,
            .ReceiveFifoLocked = DISABLE,
            .TransmitFifoPriority = DISABLE,
        },
        .State = HAL_CAN_STATE_READY,
        .ErrorCode = HAL_CAN_ERROR_NONE
    };
}
