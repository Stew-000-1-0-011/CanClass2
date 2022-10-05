#pragma once

#include "hal_can.hpp"

#include <CRSLib/std_int.hpp>
#include "can_x.hpp"

namespace CRSLib::Can::Config
{
    // フィルタバンクの個数
    inline constexpr u32 filter_bank_total_size = 14;
    // DualCANを使う場合の, スレーブの使うフィルタバンクの先頭のインデックス
    inline constexpr u32 slave_start_filter_bank = 15;

    // DualCANを使うかどうか
    inline constexpr bool is_dual_can = false;
    
    // 定義してね
    inline Can_TypeDef * can_instance(CanX can_x) noexcept
    {
        switch(can_x)
        {
        case CanX::
        }
    }

    template<>
    inline constinit CAN_TypeDef * can_instance<CanX::single_can> = 
}
