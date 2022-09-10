#pragma once

#include "main.h"

#include <CRSLib/std_int.hpp>
#include "utility.hpp"

namespace CRSLib::Can::Config
{
    // フィルタバンクの個数
    inline constexpr u32 filter_bank_total_size = 14;
    // DualCANを使う場合の、スレーブの使うフィルタバンクの先頭のインデックス
    inline constexpr u32 slave_start_filter_bank = 15;

    // DualCANを使うかどうか
    inline constexpr bool is_dual_can = false;

    // CubeMXの自動生成のコードでHAL_CAN_Initを呼んでいるかどうか
    inline constexpr bool use_cube_mx_can_init = true;
    
    // 特殊化してね
    template<CanX can_x>
    inline constexpr CAN_TypeDef * can_instance{nullptr};

    template<>
    inline constexpr CAN_TypeDef * can_instance<CanX::single_can> = static_cast<CAN_TypeDef *>(static_cast<i_ptr>(CAN1));
}