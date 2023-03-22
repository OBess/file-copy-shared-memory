#pragma once
#ifndef __INCLUDE_PRODUCER_HPP__
#define __INCLUDE_PRODUCER_HPP__

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

    class producer
    {
    public:
        producer(const std::string &in_filepath)
            : _inFile{in_filepath, std::ios_base::binary}
        {
            if (_inFile)
            {
                _remainedSymbols = new std::size_t(std::filesystem::file_size(in_filepath));
            }
        }

        ~producer()
        {
            shm::remove();
        }

        inline void run()
        {
            if (!_inFile)
            {
                my::log::logger()->error("[Producer] Cannot run copping because input filepath is invalid!");
                return;
            }

            _startProducer = shm::getStartProducer();

            _endConsumer = shm::getEndConsumer();

            std::size_t value = *_remainedSymbols;

            delete _remainedSymbols;
            _remainedSymbols = shm::setRemainedSymbols(value);

            _bufferQueue = shm::setBuffer();

            readFromFile();

            my::log::logger()->info("[Producer] Producing done!");
        }

    private:
        inline void readFromFile()
        {
            *_startProducer = true;

            my::log::logger()->flush();
            my::log::logger()->info("[Producer] Start processing...");

            while (_inFile)
            {
                if (_bufferQueue->readSize == 0)
                {
                    bi::scoped_lock lk(_mtx);

                    _inFile.read(_bufferQueue->data, _bufferQueue->bufferSize);
                    _bufferQueue->readSize = _inFile.gcount();
                }
            }

            while (*_endConsumer == false)
            {
                my::log::logger()->info("[Producer] Waiting for ending consumer process...");
            }
        }

    private:
        std::ifstream _inFile;

        std::size_t *_remainedSymbols{nullptr};

        buffer *_bufferQueue{nullptr};
        static constexpr uint32_t _sizeOfBufferQueue{100};

        bool *_startProducer{nullptr};
        bool *_endConsumer{nullptr};

        bi::named_mutex _mtx{bi::open_or_create, shm::MutexName};
        bi::named_condition _condVar{bi::open_or_create, shm::CondVarName};
    };

} // namespace inter

#endif // __INCLUDE_PRODUCER_HPP__