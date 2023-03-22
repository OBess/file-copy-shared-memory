#pragma once
#ifndef __INCLUDE_BUFFER_HPP__
#define __INCLUDE_BUFFER_HPP__

#include <thread>

namespace inter
{

    struct buffer
    {
        static constexpr std::size_t bufferSize =
            std::hardware_destructive_interference_size * 100 - sizeof(std::size_t);

        char data[bufferSize]{};
        std::size_t readSize{};
    };

    struct status
    {
        bool startProducing{false};
        bool endConsuming{false};
    };

} // namespace detail

#endif // __INCLUDE_BUFFER_HPP__