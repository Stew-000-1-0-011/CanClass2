// CMSISを使っている.
// C++標準では保証しきれないコードであるので、不用意に使いまわさないこと(stm32f1系なら多分動くんじゃないかな...)

#pragma once

#include <cstring>
#include <optional>

#include <CRSLib/interrupt_lock.hpp>
#include <CRSLib/std_int.hpp>

#include "utility.hpp"


namespace CRSLib::Can
{
	template<size_t n>
	class CanCircularQueue final
	{
		// バッファ.
		DataField buffer[n]{};

		size_t begin{0};
		size_t end{n};

	public:
		// 割り込み禁止が全然許容できそうなのでボツ. 割と面白いコードだったんだけどな...
		// 
		// // 二つの変数を関係性を崩さずに扱いたい... -> 難しい...
		// // --> せや！一つにしたろ！！(脳筋)
		// // 上位16bitがbegin, 下位16bitがend
		// volatile u32 range{0 << 8_u32 + n};

		// 割り込み安全にデータをプッシュする.
		void push(const DataField& data) noexcept
		{
			InterruptLock lock{};

			if(end == n) end = 0;

			std::memcpy(buffer[end].data(), data.data(), can_mtu);
			
			if(end == n - 1) end = 0;
			else ++end;

			if(begin == end)
			{
				if(begin == n - 1) begin = 0;
				else ++begin;
			}
		}

		// 割り込み安全にpopする.
		std::optional<DataField> pop() noexcept
		{
			InterruptLock lock{};

			if(end == n)
			{
				return std::nullopt;
			}

			DataField ret;
			std::memcpy(ret.data(), buffer[begin].data(), can_mtu);

			if(begin == n - 1) begin = 0;
			else ++begin;

			if(begin == end)
			{
				end = n;
			}

			return ret;
		}

		// 割り込み安全にキューを空にする.
		void clear() noexcept
		{
			InterruptLock lock{};
			end = begin;
		}
	};
}