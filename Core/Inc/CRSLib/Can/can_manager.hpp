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
#include "rx_unit.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can
{
	template<CanX can_x>
	class CanManager final
	{
	public:
		static void initialize(const u32 ... can_iterrupts) noexcept
		{
			// 既にCubeMXで初期化されている場合、再初期化
			if constexpr(Config::use_cube_mx_can_init)
			{
				if(HAL_CAN_DeInit(Config::can_instance<can_x>) != HAL_OK)
				{
					Debug::set_error("Fil to deinitialize bxCAN.");
					Error_Handler();
					return;
				}
			}

			// bxCANを初期化モードに変更し、初期化
			if(HAL_CAN_Init(Config::can_instance<can_x>) != HAL_OK)
			{
				Debug::set_error("Fail to initialize bxCAN.");
				Error_Handler();
				return;
			}

			// 割り込みするなら有効化
			if(activate_interrupt)
			{
				if((HAL_CAN_ActivateNotification(can_iterrupts) != HAL_OK) || ...)  // 多分short circuit. zero lengthで||はfalse.
				{
					Debug::set_error("Fail to activate CAN interrupt.");
					Error_Handler();
					return;
				}
			}

			// 動作モードに変更
			if(HAL_CAN_Start(Config::can_instance<can_x>) != HAL_OK)
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
	};
}
