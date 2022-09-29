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
			auto for_body_par_tx_unit = [this]<size_t index, size_t n>(CompileForIndex<index, n>) constexpr noexcept
			{
				CompileForIndex<index + 1, n> ret{};

				std::get<index>(tx_units).template transmit<can_x>();
				
				return ret;
			};

			compile_for(for_body_par_tx_unit, CompileForIndex<0_size_t, sizeof...(OffsetIdsEnums)>{});
		}

		template<size_t index>
		constexpr auto& get_tx_unit() noexcept
		{
			return std::get<index>(tx_units);
		}

	private:
		bool is_tx_units_not_overlap() noexcept
		{
			using namespace CRSLib::IntegerLiterals;

			bool ret{true};

			if constexpr(sizeof...(OffsetIdsEnums) > 1)
			{
				auto for_body_i = [this, &ret]<size_t i, size_t n>(CompileForIndex<i, n>) noexcept
				{
					CompileForIndex<i + 1, n> ret_i{};

					auto for_body_j = [this, &ret]<size_t j, size_t n2>(CompileForIndex<j, n2>) noexcept
					{
						CompileForIndex<j + 1, n2> ret_j{};

						if(is_overlap(std::get<i>(tx_units), std::get<j>(tx_units)))
						{
							ret = false;
							ret_j.is_breaked = true;
							return ret_j;
						}

						return ret_j;
					};

					compile_for(for_body_j, CompileForIndex<i + 1, sizeof...(OffsetIdsEnums)>{});

					return ret_i;
				};

				compile_for(for_body_i, CompileForIndex<0_size_t, sizeof...(OffsetIdsEnums) - 1>{});
			}

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
