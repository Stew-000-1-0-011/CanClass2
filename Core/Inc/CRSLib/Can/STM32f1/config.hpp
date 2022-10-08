#pragma once

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>

#include "hal_can.hpp"
#include "utility.hpp"

namespace CRSLib::Can::Config
{
    // フィルタバンクの個数
    inline constexpr u32 filter_bank_total_size = 14;

    // DualCANを使うかどうか
    inline constexpr bool use_dual_can = false;
}
