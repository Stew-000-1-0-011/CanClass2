// どのヘッダでも使うような機能はここに。
// 依存性が複雑になるので, なるべく追加しないこと。

#pragma once

#include <array>

#include <CRSLib/std_int.hpp>
#include <main.h>

namespace CRSLib::Can
{
	enum class CanX
	{
		single_can,
		can1,
		can2
	};

	inline constexpr u32 can_mtu = 8;
	inline constexpr u32 max_std_id = 0x07'FF;
	inline constexpr u32 max_ext_id = 0x03'FF'FF;
	inline constexpr u32 null_id = max_ext_id;

	using DataField = std::array<u8, can_mtu>;

	enum class FifoIndex : u32
	{
		fifo0 = 0,
		fifo1 = 1
	};

	inline constexpr bool is_in_std_id_range(const u32 id) noexcept
	{
		return id <= max_std_id;
	}
}
