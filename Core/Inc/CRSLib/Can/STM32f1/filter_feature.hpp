#pragma once

#include <array>

#include <CRSLib/std_int.hpp>

namespace CRSLib::Can
{
	enum class FilterWidth : u8
	{
		bit32,
		bit16
	};

	enum class FilterMode : u8
	{
		mask,
		list
	};

	template<FilterWidth>
	struct FrameFeature;

	// フレームの特徴量.
	// valueはそのままCAN_FilterTypeDefの.*Highや.*Lowに突っ込める.
	// 32bitのFrameFeatureはフィルタの特徴量を全て指定するが、16bitはext_idの下位15ビットを無視する.
	// 受理するIDの集合を比較するために32bitのMaskedFrameFeatureにキャストできる.
	template<>
	struct FrameFeature<FilterWidth::bit32> final
	{
		u32 value;

		struct UnPack final
		{
			u16 std_id;
			u32 ext_id;
			bool ide;
			bool rtr;
		};

		constexpr FrameFeature(const u16 std_id = max_std_id, const u32 ext_id = 0, const bool ide = false, const bool rtr = false) noexcept:
			value{std_id << (u16)21 | ext_id << (u32)8 | (u8)ide << 2 | (u8)rtr << 1}
		{}

		constexpr UnPack unpack() const noexcept
		{
			return {.std_id = value >> (u32)21, .ext_id = value >> (u32)8 & max_ext_id, .ide = value >> (u32)2 & 0b1, .rtr = value >> (u32)1 & 0b1};
		}

		friend bool operator==(const FrameFeature&, const FrameFeature&) = default;

		explicit inline constexpr operator MaskedFrameFeature<FilterWidth::bit32>() const noexcept;
	};

	template<>
	struct FrameFeature<FilterWidth::bit16> final
	{
		u16 value;

		struct UnPack final
		{
			u16 std_id;
			bool ide;
			bool rtr;
			u8 ext_id_0b111;
		};

		constexpr FrameFeature(const u16 std_id = max_std_id, const bool ide = false, const bool rtr = false, const u8 ext_id_0b111 = 0) noexcept:
			value{std_id << (u16)5 | (u8)ide << 4 | (u8)rtr << 3 | ext_id_0b111}
		{}

		constexpr UnPack unpack() const noexcept
		{
			return {.std_id = value >> (u16)5, .ide = value >> (u32)4 & 0b1, .rtr = value >> (u32)3 & 0b1, .ext_id_0b111 = value & 0b111};
		}

		friend bool operator==(const FrameFeature&, const FrameFeature&) = default;

		explicit inline constexpr operator MaskedFrameFeature<FilterWidth::bit32>() const noexcept;
	};

	// マスクによりフレームの特徴量をグループにしたもの. フレームの特徴量の集合.
	// 集合なので大小を比較できる. 実際にはstd::array<MaskedFrameFeature<FilterWidth::bit32>, n>に変換されてから比較される.
	template<FilterWidth filter_width>
	struct MaskedFrameFeature final
	{
		FrameFeature<filter_width> id;
		FrameFeature<filter_width> mask;

		friend constexpr bool operator==(const MaskedFrameFeature&, const MaskedFrameFeature&) noexcept
		{
			return l.id.value & l.mask.value == r.id.value & r.mask.value;
		}

		friend constexpr std::partial_ordering operator<=(const MaskedFrameFeature& l, const MaskedFrameFeature& r) noexcept
		{
			return !((l.mask.value | r.mask.value) ^ r.mask.value);
		}

		friend constexpr std::partial_ordering operator<=>(const MaskedFrameFeature& l, const MaskedFrameFeature& r) noexcept
		{
			return l == r ? std::partial_ordering::equivalent :
				l <= r ? std::partial_ordering::less :
				r <= l ? std::partial_ordering::greater :
				std::partial_ordering::unordered;
		}

		// filter_width == FilterWidth::bit16のときにしか呼ばれない
		explicit constexpr operator MaskedFrameFeature<FilterWidth::bit32>() const noexcept
		{
			auto id_unpack = id.unpack();
			auto mask_unpack = mask.unpack();
			return
			{
				.id = {id_unpack.std_id, id_unpack.ext_id_0b111 << (u32)15, id_unpack.ide, id_unpack.rtr},
				.mask = {mask_unpack.std_id, mask_unpack.ext_id_0b111 << (u32)15, mask_unpack.ide, mask_unpack.rtr}
			};
		}
	};

	template<>
	FrameFeature<FilterWidth::bit32>::operator MaskedFrameFeature<FilterWidth::bit32>() const noexcept
	{
		return
		{
			.id = *this,
			.mask = {max_std_id, max_ext_id, true, true}
		};
	}

	template<>
	FrameFeature<FilterWidth::bit16>::operator MaskedFrameFeature<FilterWidth::bit32>() const noexcept
	{
		auto [std_id, ide, rtr, ext_id_0b111] = unpack();
		return
		{
			.id = {std_id, ext_id_0b111 << (u32)15, ide, rtr},
			.mask = {max_std_id, (u32)(0b111 << 15), true, true}
		};
	}

	// アルゴリズム力がなくて実装できなかった. できる人お願い
	template<size_t n1, size_t n2>
	[[deprecated("I cannot Implement this.")]] inline constexpr std::partial_ordering operator<=>(const std::array<MaskedFrameFeature<FilterWidth::bit32>, n1>& l, const std::array<MaskedFrameFeature<FilterWidth::bit32>, n2>& r) noexcept;
}