// CMSISを使っている.
// C++標準では保証しきれないコードであるので、不用意に使いまわさないこと(stm32f1系なら多分動くんじゃないかな...)

#pragma once

#include <utility>
#include <optional>

#include <CRSLib/cmsis_for_cpp.h>
#include <CRSLib/std_int.hpp>

#include "utility.hpp"


namespace CRSLib::Can
{
    template<size_t n>
    class CanCircularQueue final
    {
        // バッファ.
        char buffer[n][can_mtu]{};
        
        // 割り込み禁止が全然許容できそうなのでボツ. 割と面白いコードだったんだけどな...
        // 
        // // 二つの変数を関係性を崩さずに扱いたい... -> 難しい...
        // // --> せや！一つにしたろ！！(脳筋)
        // // 上位16bitがbegin, 下位16bitがend
        // volatile u32 range{0 << 8_u32 + n};

        size_t begin{0};
        size_t end{n};

    public:
        CanCircularQueue() = default;

        // 
        void push(const char (&data)[can_mtu]) noexcept
        {
            // // このイディオム、割り込みが起きても上手く動くのかな？なんで上手くいくのかよくわからない...
            // // 割り込みから復帰時にどっかからレジスタの中身もリロードしてて、そこにll/scに関連する状態が残っているからか？
            // // ...
            // // ......そもそも割り込み禁止を許容したのにこんなことする必要あるか...？
            // // -> 書いてて楽しいからいいんだよ！！
            // // --> でも割り込み禁止を使うほうがなんか早そう(未計測)なのでやめた...
            // 
            // u16 push_index;
            // u32 current_range;
            // do
            // {
            //     current_range = stew_load_link_32(&range);

            //     u16 current_begin = current_range & 0xFF'FF'00'00_u32;
            //     u16 current_end = push_index = current_range >> 8_u32;

            //     if(current_end >= n - 1) current_end -= n - 1;
            //     else ++current_end;

            //     if(current_end == current_begin)
            //     {
            //         if(current_begin == n - 1) current_begin = 0;
            //         else ++current_begin;
            //     }

            //     current_range = current_begin << 8_u32 | current_end;
            // }while(stew_store_conditional_32(&range, current_range));

            stew_disable_irq();

            if(end == n) end = 0;

            std::memcpy(buffer + end, data, can_mtu);
            
            if(end == n - 1) end = 0;
            else ++end;

            if(begin == end)
            {
                if(begin == n - 1) begin = 0;
                else ++begin;
            }

            stew_enable_irq();
        }

        std::optional<T> pop() noexcept
        {
            stew_disable_irq();

            if(end == n)
            {
                return std::nullopt;
            }

            char ret[can_mtu];
            std::memcpy(ret, buffer + begin, can_mtu);

            if(begin == n - 1) begin = buffer;
            else ++begin;

            if(begin == end)
            {
                end = n;
            }

            stew_enable_irq();

            return ret;
        }

        void clear() noexcept
        {
            stew_disable_irq();
            end = begin;
            stew_enable_irq();
        }
    };
}