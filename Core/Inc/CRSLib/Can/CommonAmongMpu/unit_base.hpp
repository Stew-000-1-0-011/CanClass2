#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>
#include <CRSLib/debug.hpp>

namespace CRSLib::Can
{
	// idの範囲チェックに関する機能を有する.
	template<OffsetIdsEnumC OffsetIdsEnum_>
	class UnitBase
	{
		using OffsetIdsEnum = OffsetIdsEnum_;
	public:
		static constexpr auto id_num = to_underlying(OffsetIdsEnum::n);
		// static constexpr auto align = UnitBase<OffsetIdsEnum>::calc_id_align(id_num);
		u32 base_id;

	public:
		UnitBase(const u32 base_id) noexcept:
			base_id{base_id}
		{
			// if(!(base_id % align))
			// {
			// 	Debug::set_error("base_id does not meet the required alignment.");
			// 	Error_Handler();
			// }
		}

	public:
		bool is_in(const u32 id) const noexcept
		{
			return base_id < id && id < base_id + id_num;
		}

		template<OffsetIdsEnumC OffsetIdsEnum2>
		friend constexpr bool is_overlap(const UnitBase& l, const UnitBase<OffsetIdsEnum2>& r) noexcept
		{
			return l.base_id < r.base_id + r.id_num && r.base_id < l.base_id + l.id_num;
		}

	// private:
	// 	static constexpr u32 calc_id_align(const u32 id_num) noexcept
	// 	{
	// 		u32 ret = 1;
	// 		u32 tmp = id_num;
	// 		while(tmp > 1)
	// 		{
	// 			tmp >>= 1;
	// 			ret <<= 1;
	// 		}
	// 		return ret;
	// 	}
	};
}
