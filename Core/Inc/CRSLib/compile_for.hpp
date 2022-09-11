#pragma once

namespace CRSLib
{
    template<class BodyFunc, class ForIter>
    requires requires(ForIter for_iter)
    {
        static_cast<bool>(for_iter);
    }
    constexpr auto&& compile_for(BodyFunc&& body_func, ForIter&& for_iter) noexcept
    {
        auto&& next_iter = body_func(for_iter);
        if(next_iter) return next_iter;
        else return compile_for(static_cast<BodyFunc&&>(body_func), static_cast<decltype(next_iter)&&>(next_iter));
    }

}