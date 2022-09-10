#pragma once
// c++20以降でなければならない
// なんで-std=c++20なのにstatic assertion failed するの...?
//static_assert(__cplusplus >= 202002L);

#include <cstring>
#include <bit>
#include <type_traits>
#include <utility>

#include <CRSLib/std_int.hpp>
#include "utility.hpp"

namespace CRSLib::Can
{
    template<class T>
    concept BeAbleToPack = sizeof(T) <= can_mtu && std::is_trivially_copyable_v<T>;

    template<BeAbleToPack T, std::endian endian>
    inline void pack(char (&buffer)[can_mtu], const T& value) noexcept
    {
        std::memcpy(buffer, &value, sizeof(T));

        if constexpr(std::endian::native != endian)
        {
            for(unsigned int i = 0; i < sizeof(T) / 2; ++i)
            {
                std::swap(buffer[i], buffer[sizeof(T) - 1 - i]);
            }
        }
    }

    template<BeAbleToPack T, std::endian endian>
    inline T unpack(const char (&buffer)[can_mtu]) noexcept
    {
        if constexpr(std::endian::native != endian)
        {
        	char tmp[can_mtu];
            for(unsigned int i = 0; i < sizeof(T); ++i)
            {
                tmp[i] = buffer[sizeof(T) - 1 - i];
            }

            // 確か大丈夫だったはず(char *からは任意の型にアクセスできるんじゃないっけ？)
            return *static_cast<T *>(tmp);
        }
        else
        {
        	return *static_cast<T *>(buffer);
        }
    }


}
