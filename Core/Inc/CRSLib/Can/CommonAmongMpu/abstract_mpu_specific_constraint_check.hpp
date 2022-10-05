#pragma once

#include "abstruct_mpu_specific_interface.hpp"
#include <CRSLib/Can/STM32f1/abstract_mpu_specific_iplement.hpp>

namespace CRSLib::Can
{
	static_assert(TxFrameC<TxFrame>);
	static_assert(PillarboxC<Pillarbox, TxFrame>);
	static_assert(RxFrameC<RxFrame>);
	static_assert(LetterboxC<Letterbox, RxFrame>);
}