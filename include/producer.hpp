#pragma once
#ifndef __INCLUDE_PRODUCER_HPP__
#define __INCLUDE_PRODUCER_HPP__

#include <fstream>
#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "config.hpp"

namespace inter
{

    class producer
    {
    public:
        producer(const std::string &in_filepath)
            : _filepath{in_filepath}
        {
        }

        inline void run()
        {
            std::ifstream inFile{_filepath, std::ios_base::binary};

            if (!inFile)
            {
                std::cout << "Cannot run copping because input filepath is invalid!\n";
                return;
            }

            const std::size_t remainedSymbols = std::filesystem::file_size(_filepath);
        }

    private:
        std::string _filepath;
    };

} // namespace inter

#endif // __INCLUDE_PRODUCER_HPP__