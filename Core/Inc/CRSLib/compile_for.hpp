#pragma once

#include <concepts>

namespace CRSLib
{
    namespace Implement::CompileFor
    {
        template<class T>
        concept IsForIter = requires(T iter)
        {
            requires std::same_as<decltype(iter.is_breaked), bool>;
        };
    }

    template<class BodyFunc, Implement::CompileFor::IsForIter ForIter>
    constexpr auto&& compile_for(BodyFunc&& body_func, ForIter&& for_iter) noexcept
    {
        auto&& next_iter = body_func(for_iter);
        if(next_iter) return next_iter;
        else return compile_for(static_cast<BodyFunc&&>(body_func), static_cast<decltype(next_iter)&&>(next_iter));
    }

    template<auto index>
    struct CompileForIndex final
    {
        bool is_breaked{false};
    };
}