#pragma once

#include <tuple>

#include <main.h>

#include <CRSLib/interrupt_disabler.hpp>
#include <CRSLib/compile_for.hpp>

#include "utility.hpp"
#include "offset_id.hpp"
#include "rx_unit.hpp"
#include "filter_manager.hpp"

namespace CRSLib::Can
{
	template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnums>
	class Receiver final
	{
		std::tuple<RxUnit<OffsetIdsEnums> * ...> rx_units{};
		
		friend constexpr Receiver make_receiver(RxUnit<OffsetIdsEnums>& ... arg_rx_units) noexcept
		{
			return Receiver{arg_rx_units};
		}

		constexpr Receiver(RxUnit<OffsetIdsEnums>& ... arg_rx_units) noexcept:
			rx_units{&arg_rx_units ...}
		{
			using namespace Implement::ReceiverImp;
			using namespace IntegerLiterals;

			if(!are_correctly_lined_up(arg_rx_units ...))
			{
				Debug::set_error("There are duplicates or they are not lined up correctly.");
				Error_Handler();
				goto end;
			}

			auto for_body_par_rx_unit = [this]<size_t index>(CompileForIndex<index>)
			{
				CompileForIndex<index + 1> ret{};
				if constexpr(index == sizeof...(OffsetIdsEnums))
				{
					ret.is_breaked = true;
					return ret;
				}

				FilterManager::register_filter(std::get<index>(rx_units)->get_filter_id(), true);

				return ret;
			}

			compile_for(for_body_par_rx_unit, CompileForIndex<0_size_t>{});

		end:
		}

		// 各rx_unitsのキューに受信したフレームを振り分ける.
		template<FifoIndex fifo_index>
		void receive() noexcept
		{
			constexpr u32 fifo = static_cast<u32>(fifo_index);

			while(true)
			{
				RxFrame rx_frame{};
				CAN_RxHeaderTypeDef rx_header{};

				{
					InterruptDisabler disabler{};
					if(HAL_CAN_GetRxFifoFillLevel(&hcan<can_x>, fifo) != 0)
					{
						HAL_CAN_GetRxMessage(&hcan<can_x>, fifo, &rx_header, rx_frame.data.data());
					}
					else return;
				}

				rx_frame.header.id = rx_header.IDE == CAN_ID_STD ? rx_header.StdId : rx_header.ExtId;
				rx_frame.header.time_stamp = rx_header.Timestamp;
				rx_frame.header.is_remote = rx_header.RTR == CAN_RTR_REMOTE;
				rx_frame.header.dlc = rx_header.DLC;

				auto for_body_par_rx_unit = [this, &rx_frame]<size_t index>(CompileForIndex<index>) noexcept
				{
					CompileForIndex<index + 1> ret{};
					if constexpr(index == sizeof...(OffsetIdsEnums))
					{
						ret.is_breaked = true;
						return ret;
					}

					if(std::get<index>(rx_units)->push_queue(rx_frame))
					{
						ret.is_breaked = true;
						return ret;
					}

					return ret;
				};

				compile_for(for_body_par_rx_unit, CompileForIndex<0_size_t>{});
			}
		}

		// 一つ以上フレームを格納しているrx_idのうち, もっとも番号の若いrx_idのコールバックを1つ呼び出す. 無ければ何もしない.
		void call() noexcept
		{
			auto for_body_par_unit = [this]<size_t index>(CompileForIndex<index>) noexcept
			{
				CompileForIndex<index + 1> ret{};
				if constexpr(index == sizeof...(OffsetIdsEnums))
				{
					ret.is_breaked = true;
					return ret;
				}

				if(std::get<index>(rx_units)->call_once())
				{
					ret.is_breaked = true;
					return ret;
				}

				return ret;
			};

			compile_for(for_body_par_unit, CompileForIndex<0_size_t>{});
		}
	};
}