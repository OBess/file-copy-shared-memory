#pragma once
#ifndef __INCLUDE_CONSUMER_HPP__
#define __INCLUDE_CONSUMER_HPP__

#include <fstream>
#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "config.hpp"

namespace inter
{

    class consumer
    {
    public:
        consumer(const std::string &out_filepath)
            : _filepath{out_filepath}
        {
        }

        inline void run()
        {
            std::ofstream outFile{_filepath, std::ios_base::binary};

            if (!outFile)
            {
                std::cout << "Cannot run copping because output filepath is invalid!\n";
                return;
            }
        }

    private:
        std::string _filepath;

        std::size_t _remainedSymbols{};
    };

} // namespace inter

#endif // __INCLUDE_CONSUMER_HPP__