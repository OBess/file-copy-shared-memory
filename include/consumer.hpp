#pragma once
#ifndef __INCLUDE_CONSUMER_HPP__
#define __INCLUDE_CONSUMER_HPP__

#include <fstream>
#include <iostream>
#include <queue>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "buffer.hpp"
#include "shared_memory.hpp"
#include "logger.hpp"

namespace inter
{
    namespace bi = boost::interprocess;

    class consumer
    {
    public:
        consumer(const std::string &out_filepath)
            : _outFile{out_filepath, std::ios_base::binary}
        {
        }

        inline void run()
        {
            if (!_outFile)
            {
                my::log::consumer_logger()->error("[Consumer] Cannot run copping because output filepath is invalid!");
                return;
            }

            _status = shm::getStatus();

            my::log::consumer_logger()->info("[Consumer] Waiting for staring producer process...");
            while (_status->startProducing == false)
            {
            }

            _remainedSymbols = shm::getRemainedSymbols();
            _bufferQueue = shm::getBuffer();

            writeToFile();

            my::log::consumer_logger()->info("[Consumer] Consuming done!");
        }

    private:
        inline void writeToFile()
        {
            my::log::consumer_logger()->info("[Consumer] Start processing...");
            my::log::consumer_logger()->flush();

            while (*_remainedSymbols > 0)
            {
                if (_bufferQueue->readSize > 0)
                {
                    bi::scoped_lock lk(_mtx);

                    _outFile.write(_bufferQueue->data, _bufferQueue->readSize);

                    *_remainedSymbols -= _bufferQueue->readSize;
                    _bufferQueue->readSize = 0;
                }
            }

            _status->endConsuming = true;
        }

    private:
        std::ofstream _outFile;

        std::size_t *_remainedSymbols{nullptr};

        buffer *_bufferQueue{nullptr};
        static constexpr uint32_t _sizeOfBufferQueue{100};

        status *_status{nullptr};

        bi::named_mutex _mtx{bi::open_or_create, shm::MutexName};
    };

} // namespace inter

#endif // __INCLUDE_CONSUMER_HPP__