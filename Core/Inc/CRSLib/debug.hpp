#pragma once

#include <CRSLib/std_int.hpp>

namespace CRSLib::Debug
{
    namespace Implement::DebugImp
    {
        inline constinit bool error_happened{false};
        inline constinit const char * error_message{nullptr};
    }

    template<size_t str_size>
    inline void set_error(const char(&str)[str_size] = "Error happened.") noexcept
    {
        Implement::DebugImp::error_happened = true;
        if(!Implement::DebugImp::error_message) Implement::DebugImp::error_message = str;
    }
}