// どのヘッダでも使うような機能はここに。
// 依存性が複雑になるので, なるべく追加しないこと。

#pragma once

#include <array>

#include <CRSLib/std_int.hpp>

namespace CRSLib::Can
{
	inline constexpr u32 can_mtu = 8;
	inline constexpr u32 max_std_id = 0x07'FF;
	inline constexpr u32 max_ext_id = 0x03'FF'FF;
	inline constexpr u32 null_id = max_ext_id;

	using DataField = std::array<u8, can_mtu>;
}
