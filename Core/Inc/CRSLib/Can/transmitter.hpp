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
#include "offset_id.hpp"
#include "tx_unit.hpp"

namespace CRSLib::Can
{
	template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnums>
	class Transmitter final
	{
		std::tuple<TxUnit<OffsetIdsEnums> ...> tx_units{};

	public:
		Transmitter() noexcept
		{
			assert_not_overlap();
		}

	public:
		void transmit() noexcept
		{
			using namespace IntegerLiterals;

			// for文って偉大だね...
			auto for_body_par_tx_unit = [this]<size_t index>(CompileForIndex<index>) constexpr noexcept
			{
				CompileForIndex<index + 1> ret{};
				if constexpr(index == sizeof...(OffsetIdsEnums))
				{
					ret.is_breaked = true;
					return ret;
				}

				std::get<index>(tx_units).transmit();
				
				return ret;
			};

			compile_for(for_body_par_tx_unit, CompileForIndex<0_size_t>{});
		}

		template<size_t index>
		constexpr auto& get_tx_unit() noexcept
		{
			return std::get<index>(tx_units);
		}

	private:
		bool is_tx_units_not_overlap() noexcept
		{
			bool ret{true};
			auto for_body_i = [this, &ret]<size_t i>(CompileForIndex<i>) noexcept
			{
				CompileForIndex<i + 1> ret_i{};

				if constexpr(i == sizeof...(OffsetIdsEnums) - 1)
				{
					ret_i.is_breaked = true;
					return ret_i;
				}

				auto for_body_j = [this, &ret]<size_t j>(CompileForIndex<j>) noexcept
				{
					CompileForIndex<j + 1> ret_j{};

					if constexpr(j == sizeof...(OffsetIdsEnums))
					{
						ret_j.is_breaked = true;
						return ret_j;
					}

					if(is_overlap(std::get<i>(tx_units), std::get<j>(tx_units)))
					{
						ret = false;
						ret_j.is_breaked = true;
						return ret_j;
					}

					return ret_j;
				};

				compile_for(for_body_j, CompileForIndex<i + 1>{});

				return ret_i;
			};

			compile_for(for_body_i, CompileForIndex<0_size_t>{});

			return ret;
		}

		void assert_not_overlap() noexcept
		{
			if(!is_tx_units_not_overlap())
			{
				Debug::set_error("Tx Unit must not overlap.");
				Error_Handler();
			}
		}
	};
}