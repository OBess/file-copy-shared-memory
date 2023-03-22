#pragma once
#ifndef __INCLUDE_LOGGER_HPP__
#define __INCLUDE_LOGGER_HPP__

#include "spdlog/sinks/basic_file_sink.h"

namespace my::log
{

    static inline auto logger()
    {
        static auto instance(spdlog::basic_logger_mt("file-copy-shared-memory", "logs/logs.txt"));
        return instance;
    }

} // namespace log

#endif // __INCLUDE_LOGGER_HPP__