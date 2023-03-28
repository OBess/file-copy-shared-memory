#pragma once
#ifndef __INCLUDE_EXCEPTION_HPP__
#define __INCLUDE_EXCEPTION_HPP__

#include <stdexcept>
#include <string_view>

namespace shm
{

    class exception : public std::runtime_error
    {
    public:
        using base = std::runtime_error;

        explicit exception(std::string_view msg)
            : base(msg.data())
        {
        }
    };

} // namespace shm

#endif // __INCLUDE_EXCEPTION_HPP__