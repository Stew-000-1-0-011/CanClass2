#pragma once

#include <bit>
#include <concepts>
#include <tuple>
#include <utility>

#include "main.h"

#include <CRSLib/std_int.hpp>
#include <CRSLib/to_value.hpp>
#include "pack.hpp"
#include "config.hpp"
#include "utility.hpp"
#include "can_unit.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can
{
	template<CanX can_x>
	class CanManager final
	{

		CanManager() noexcept:
		{
			// 既にCubeMXで初期化されている場合、再初期化
			if constexpr(Config::use_cube_mx_can_init)
			{
				if(HAL_CAN_DeInit(&hcan<can_x>) != HAL_OK)
				{
					Debug::set_error("Fil to deinitialize bxCAN.");
					Error_Handler();
				}
			}

			// bxCANを初期化モードに変更し、初期化
			if(HAL_CAN_Init(hcan) != HAL_OK)
			{
				Debug::set_error("Fail to initialize bxCAN.");
				Error_Handler();
			}

			// 動作モードに変更
			HAL_CAN_Start(hcan);
		}

	public:
		void make_silent() noexcept
		{}

		void make_normal() noexcept
		{}

		void spin_once() const noexcept
		{

		}

	private:
		
	};
}
