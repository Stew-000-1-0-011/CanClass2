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
	template<IsOffsetIdsEnum ... OffsetIdsEnums>
	class Receiver final
	{
		std::tuple<Implement::RxUnit<OffsetIdsEnums> ...> rx_units{};

	public:
		// 各rx_unitsに受信したフレームを振り分け, executorにコールバックをpushする.
		template<CanX can_x, FifoIndex fifo_index, size_t queue_size>
		void receive(Executor<void () noexcept, queue_size>& executor) noexcept
		{
			using namespace CRSLib::IntegerLiterals;

			constexpr u32 fifo = static_cast<u32>(fifo_index);

			while(true)
			{
				// フレームを準備
				RxFrame rx_frame{};
				CAN_RxHeaderTypeDef rx_header{};

				{
					InterruptDisabler disabler{};
					if(HAL_CAN_GetRxFifoFillLevel(&Implement::hcan<can_x>, fifo) != 0)
					{
						HAL_CAN_GetRxMessage(&Implement::hcan<can_x>, fifo, &rx_header, rx_frame.data.data());
					}
					else return;
				}

				rx_frame.header = make_rx_header(rx_header);

				// rx_units内のrx_unitそれぞれのreceiveを呼び出す
				auto for_body_par_rx_unit = [this, &rx_frame, &executor]<size_t index, size_t n>(CompileForIndex<index, n>) noexcept
				{
					CompileForIndex<index + 1, n> ret{};

					std::get<index>(rx_units).receive(rx_frame, executor);

					return ret;
				};

				compile_for(for_body_par_rx_unit, CompileForIndex<0_size_t, sizeof...(OffsetIdsEnums)>{});
			}
		}

		template<size_t index>
		auto& get() noexcept
		{
			return std::get<index>(rx_units);
		}
	};
}
