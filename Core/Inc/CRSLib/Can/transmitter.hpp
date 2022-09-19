#pragma once

#include <utility>
#include <type_traits>
#include <tuple>

#include <main.h>

#include <CRSLib/compile_for.hpp>
#include <CRSLib/debug.hpp>
#include <CRSLib/interrupt_disabler.hpp>

#include "config.hpp"
#include <CRSLib/interrupt_safe_circular_queue.hpp>
#include "offset_id_impl_injector.hpp"
#include "offset_id.hpp"
#include "tx_unit.hpp"

namespace CRSLib::Can
{
	template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnums>
	class Transmitter final
	{
		std::tuple<TxUnit<OffsetIdsEnums> * ...> tx_units{};

		template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnums>
		friend constexpr Transmitter make_transmitter(TxUnit<OffsetIdsEnums>& ... arg_tx_units) noexcept
		{
			return Transmitter{arg_tx_units};
		}

		constexpr Transmitter(TxUnit<OffsetIdsEnums>& ... arg_tx_units) noexcept:
			tx_units{&arg_tx_units ...}
		{
			if(!are_correctly_lined_up(arg_tx_units ...))
			{
				Debug::set_error("There are duplicates or they are not lined up correctly.");
				Error_Handler();
			}
		}

	public:
		void transmit() noexcept
		{
			using Implement::TransmitterImp;
			using namespace IntegerLiterals;

			// for文って偉大だね...
			auto for_body_par_tx_unit = [this]<size_t index>(CompileForIndex<index>) constexpr noexcept
			{
				CompileForIndex<index + 1> ret{};
				if constexpr(index == sizeof...(OffsetIdsEnums)) ret.is_breaked = true;

				const auto * tx_unit_p = std::get<index>(tx_units);
				using Enum = decltype(*tx_unit_p)::OffsetIdsEnum;

				auto for_body_par_offset_id = [tx_unit_p]<Enum offset_id>(CompileForIndex<offset_id>) constexpr noexcept
				{
					CompileForIndex<static_cast<Enum>(to_underlying(offset_id) + 1)> ret{};
					if constexpr(offset_id == Enum::n)
					{
						ret.is_breaked = true;
						return ret;
					}

					
				};

				compile_for(for_body_par_offset_id, CompileForIndex<static_cast<Enum>(0)>{});
				
				return ret;
			};

			compile_for(for_body_par_tx_unit, CompileForIndex<0_size_t>{});
		}
	};
}