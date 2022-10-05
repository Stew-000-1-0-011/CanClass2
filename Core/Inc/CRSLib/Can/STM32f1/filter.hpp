#include <array>

#include "hal_can.hpp"
#include "filter_feature.hpp"

namespace CRSLib::Can
{
	template<FilterWidth, FilterMode>
	struct Filter;

	template<>
	struct Filter<FilterWidth::bit32, FilterMode::mask> final
	{
		MaskedFrameFeature<FilterWidth::bit32> masked32;
	};

	template<>
	struct Filter<FilterWidth::bit32, FilterMode::list> final
	{
		std::array<FrameFeature<FilterWidth::bit32>, 2> feature32s;
	};

	template<>
	struct Filter<FilterWidth::bit16, FilterMode::mask> final
	{
		std::array<MaskedFrameFeature<FilterWidth::bit16>, 2> masked16s;
	};

	template<>
	struct Filter<FilterWidth::bit16, FilterMode::list> final
	{
		std::array<FrameFeature<FilterWidth::bit16>, 4> feature16s;
	};

	// なんでCAN_TypeDefのFilterIdHighとかu16じゃなくてu32なの...？
	// -> あまりにあんまりなため、HAL_ConfigFilter相当の機能を自作することにした. 可搬性は同程度に維持するつもりだ.
	// DualCANでも動き, 関連の薄い変更を分離する. 
	
	// inline constexpr  make_filter(const Filter<FilterWidth::bit32, FilterMode::mask>& filter) noexcept
	// {
	// 	return
	// 	{
	// 		.FilterIdHigh = filter.masked32.id.value >> (u32)16,
	// 		.FilterIdLow = filter.masked32.id.value & (u32)0xFF'FF,
	// 		.FilterMaskIdHigh = filter.masked32.mask.value >> (u32)16,
	// 		.FilterMaskIdLow = filter.masked32.mask.value & (u32)0xFF'FF,
	// 		.FilterFIFOAssignment = filter.
	// 	};
	// }
}