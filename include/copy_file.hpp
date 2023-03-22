#pragma once
#ifndef __INCLUDE_COPY_FILE_HPP__
#define __INCLUDE_COPY_FILE_HPP__

#include <fstream>
#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "buffer.hpp"
#include "logger.hpp"
#include "status.hpp"
#include "shared_memory.hpp"

namespace inter
{
    namespace bi = boost::interprocess;

    class copy_file
    {
    public:
        copy_file(std::string_view in_filepath, std::string_view out_filepath,
                  std::string_view shared_name)
            : _inFilepath{in_filepath}, _outFilepath{out_filepath},
              _sharedName{shared_name}
        {
        }

        ~copy_file()
        {
            bi::named_mutex::remove(_mutexName.data());
            bi::shared_memory_object::remove(_sharedName.data());
        }

        /// @brief Gets pointers from shared memory and runs function that read-write
        ///        data form-to file throught shared memory
        inline void run()
        {
            // Checks if current process can be producer
            const bool isProducer = shm::exists(_sharedName) == false;

            _sharedMemory = shm::getShareMemory(_sharedName);

            _status = reinterpret_cast<status *>(_sharedMemory.ptr);
            _remainedSymbols =
                reinterpret_cast<std::size_t *>(_sharedMemory.ptr +
                                                _sharedMemory.statusSize);
            _buffer = reinterpret_cast<buffer *>(_sharedMemory.ptr +
                                                 (_sharedMemory.entireSize -
                                                  _sharedMemory.bufferSize));

            if (isProducer)
            {
                std::cout << "Producer" << std::endl;
                producer();
            }
            else
            {
                std::cout << "Consumer" << std::endl;
                consumer();
            }
        }

    private:
        /// @brief Producing data to shared memory
        inline void producer()
        {
            std::ifstream inFile{_inFilepath.data(), std::ios::binary};

            if (!inFile)
            {
                my::log::producer_logger()->error("[Producer] Cannot run copping"
                                                  " because input filepath is invalid!");
                return;
            }

            if (inFile)
            {
                *_remainedSymbols = std::filesystem::file_size(_inFilepath);
            }

            readFromFile(inFile);

            my::log::producer_logger()->info("[Producer] Producing done!");
        }

        /// @brief Reads data from file and saves to shared memory
        inline void readFromFile(std::ifstream &inFile)
        {
            _status->startProducing = true;

            while (inFile)
            {
                if (_buffer->readSize == 0)
                {
                    bi::scoped_lock lk(_mtx);

                    inFile.read(_buffer->data, _buffer->bufferSize);
                    _buffer->readSize = inFile.gcount();
                }
            }

            while (_status->endConsuming == false)
            {
                // Waiting for ending consumer process...
            }
        }

        /// @brief Consuming data from shared memory
        inline void consumer()
        {
            std::ofstream outFile{_outFilepath.data(), std::ios::binary};

            if (!outFile)
            {
                my::log::consumer_logger()->error("[Consumer] Cannot run copping"
                                                  " because output filepath is invalid!");
                return;
            }

            while (_status->startProducing == false)
            {
                // Waiting for staring producer process...
            }

            writeToFile(outFile);

            my::log::consumer_logger()->info("[Consumer] Consuming done!");
        }

        /// @brief Reads data from shared memory and saves to file
        inline void writeToFile(std::ofstream &outFile)
        {
            while (*_remainedSymbols > 0)
            {
                if (_buffer->readSize > 0)
                {
                    bi::scoped_lock lk(_mtx);

                    outFile.write(_buffer->data, _buffer->readSize);

                    *_remainedSymbols -= _buffer->readSize;
                    _buffer->readSize = 0;
                }
            }

            _status->endConsuming = true;
        }

    private:
        std::string_view _inFilepath;
        std::string_view _outFilepath;
        std::string_view _sharedName;

        shm::shared_memory _sharedMemory;

        status *_status{nullptr};
        std::size_t *_remainedSymbols{nullptr};
        buffer *_buffer{nullptr};

        std::string_view _mutexName{"COPY_FILE_MUTEX"};
        bi::named_mutex _mtx{bi::open_or_create, _mutexName.data()};
    };

} // namespace inter

#endif // __INCLUDE_COPY_FILE_HPP__