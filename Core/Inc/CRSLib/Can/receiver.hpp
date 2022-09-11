#pragma once

#include "utility.hpp"
#include "offset_id.hpp"
#include "rx_unit.hpp"

namespace CRSLib::Can
{
	template<CanX can_x, IsOffsetIdsEnum ... OffsetIdsEnum>
	class Receiver final
	{
		std::tuple<RxUnit<OffsetIdsEnum> * ...> rx_units{};
		
	};
}