#pragma once
#ifndef __INCLUDE_STATUS_HPP__
#define __INCLUDE_STATUS_HPP__

namespace inter
{

    struct status
    {
        bool startProducing{false};
        bool endProducing{false};
        bool endConsuming{false};
    };

} // namespace detail

#endif // __INCLUDE_STATUS_HPP__