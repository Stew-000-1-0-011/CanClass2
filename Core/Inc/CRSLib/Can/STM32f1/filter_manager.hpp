#pragma once

#include <array>

#include <CRSLib/std_int.hpp>

#include "utility.hpp"
#include "config.hpp"
#include "filter_feature.hpp"

namespace CRSLib::Can
{
	namespace Implement::FilterManagerImp
	{
		std::array<>

		struct Range final
		{
			u32 begin;
			u32 end;
		};

		template<CanX can_x>
		inline constexpr u32 max_filter_bank_index = (can_x == CanX::can1)?
			Config::slave_start_filter_bank - 1 :
			Config::filter_bank_total_size - Config::slave_start_filter_bank + 1;
	}
}