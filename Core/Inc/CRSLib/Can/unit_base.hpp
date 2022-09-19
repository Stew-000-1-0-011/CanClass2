#pragma once

#include <utility>
#include <tuple>

#include <CRSLib/std_int.hpp>
#include <CRSLib/utility.hpp>
#include <CRSLib/debug.hpp>

namespace CRSLib::Can
{
    template<IsOffsetIdsEnum IsOffsetIdsEnum_>
    class UnitBase
    {
    public:
		using OffsetIdsEnum = OffsetIdsEnum_;
		static constexpr auto id_num = to_underlying(OffsetIdsEnum::n);
		static constexpr auto align = UnitBase<OffsetIdsEnum>::calc_id_align(id_num);
	
	protected:
		u32 base_id;

	public:
		UnitBase(const u32 base_id) noexcept:
			base_id{base_id}
		{
			if(!(base_id % align))
			{
				Debug::set_error("base_id does not meet the required alignment.");
				Error_Handler();
			}
		}

	public:
		bool is_in(const u32 id) const noexcept
		{
			return base_id < id && id < base_id + id_num;
		}

		u32 get_base_id() const noexcept
		{
			return base_id;
		}

		template<IsOffsetIdsEnum OffsetIdsEnum2>
		friend constexpr bool is_overlap(const UnitBase& l, const UnitBase<OffsetIdsEnum2>& r) noexcept
		{
			return l.base_id < r.base_id + r.id_num && r.base_id < l.base_id + l.id_num;
		}

	private:
		// id_num > 0
		static constexpr u32 calc_id_align(const u32 id_num) noexcept
		{
			u32 ret = 1;
			u32 tmp = id_num;
			while(tmp > 1)
			{
				tmp >>= 1;
				ret <<= 1;
			}
			return ret;
		}
    };

	template<IsOffsetIdsEnum ... OffsetIdsEnum>
	inline constexpr bool are_correctly_lined_up(const UnitBase<OffsetIdsEnum>& ... units) noexcept
	{
		const auto units_former = std::make_tuple(&units ..., nullptr);
		const auto units_latter = std::make_tuple(nullptr, &units ...);

		return [&units_former, &units_latter]<size_t ... indices>(std::index_sequence<indices ...>)
		{
			return (std::get<indices + 1>(units_former) < std::get<indices + 1>(units_latter) &&
				is_overlap(std::get<indices + 1>(units_former), std::get<indices + 1>(units_latter)) && ...);
		}
		(std::make_index_sequence<sizeof...(OffsetIdsEnum)>);
	}
}