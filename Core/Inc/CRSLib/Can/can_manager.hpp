#pragma once

#include <bit>
#include <concepts>
#include <tuple>
#include <utility>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include "pack.hpp"
#include "config.hpp"
#include "utility.hpp"
#include "handle.hpp"
#include "rx_unit.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can
{
	template<CanX can_x>
	class CanManager final
	{
	public:
		static void initialize(std::same_as<const u32> auto ... can_interrupts) noexcept
		{
			// 既にCubeMXで初期化されている場合, 再初期化
			if constexpr(Config::use_cube_mx_can_init)
			{
				if(HAL_CAN_DeInit(&Implement::hcan<can_x>) != HAL_OK)
				{
					Debug::set_error("Fail to deinitialize bxCAN.");
					Error_Handler();
					return;
				}
			}

			// bxCANを初期化モードに変更し, 初期化
			if(HAL_CAN_Init(&Implement::hcan<can_x>) != HAL_OK)
			{
				Debug::set_error("Fail to initialize bxCAN.");
				Error_Handler();
				return;
			}

			// 割り込みするなら有効化
			if constexpr(sizeof...(can_interrupts))
			{
				if(((HAL_CAN_ActivateNotification(&Implement::hcan<can_x>, can_interrupts) != HAL_OK) || ...))  // 多分short circuit. zero lengthで||はfalse.
				{
					Debug::set_error("Fail to activate CAN interrupt.");
					Error_Handler();
					return;
				}
			}

			// 動作モードに変更
			if(HAL_CAN_Start(&Implement::hcan<can_x>) != HAL_OK)
			{
				Debug::set_error("Fail to start bxCAN.");
				Error_Handler();
				return;
			}
		}

		[[deprecated("unimplemented.")]] static void make_silent() noexcept
		{}

		[[deprecated("unimplemented.")]] static void make_normal() noexcept
		{}

		// 他にも, CanXの列挙子に紐づく機能全体に影響する関数を書きたいなら, ここの静的メンバ関数に加えてほしい.
	};
}
