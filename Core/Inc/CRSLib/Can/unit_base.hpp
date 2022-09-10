#pragma once

#include <utility>

#include <CRSLib/std_int.hpp>
#include <CRSLib/debug.hpp>

#include "offset_id_impl_injector.hpp"

namespace CRSLib::Can
{
    template<IsOffsetIdsEnum IsOffsetIdsEnum_>
    class UnitBase
    {
    public:
		using OffsetIdsEnum = OffsetIdsEnum_;
		static constexpr auto id_num = static_cast<std::underlying_type_t<OffsetIdsEnum>>(OffsetIdsEnum::n);
		static constexpr auto align = UnitBase<OffsetIdsEnum>::calc_id_align(id_num);
	
	protected:
		OffsetIdTuple<OffsetIdsEnum::n> offset_ids{};
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

		template<IsOffsetIdsEnum OffsetIdsEnum2>
		bool is_duplicate(const UnitBase<OffsetIdsEnum2>& obj) noexcept
		{
			return obj.base_id < base_id + id_num && base_id < obj.base_id + obj.id_num;
		}

		u32 get_base_id() const noexcept
		{
			return base_id;
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
}