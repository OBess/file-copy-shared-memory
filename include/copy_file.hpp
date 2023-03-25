#pragma once
#ifndef __INCLUDE_COPY_FILE_HPP__
#define __INCLUDE_COPY_FILE_HPP__

#include <fstream>
#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>

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
            if (_isProducer)
            {
                free_shared_memory();
            }
        }

        /// @brief Gets pointers from shared memory and runs function that read-write
        ///        data form-to file throught shared memory
        inline void run()
        {
            // Checks if current process can be producer
            _isProducer = shm::exists(_sharedName) == false;

            uint8_t *shmPtr = shm::getShareMemory(_sharedName, _isProducer);

            if (_isProducer)
            {
                _sharedMemory = new (shmPtr) shm::shared_memory;

                producer();
            }
            else
            {
                _sharedMemory = reinterpret_cast<shm::shared_memory *>(shmPtr);

                consumer();
            }
        }

    private:
        /// @brief Frees allocated shared memory
        inline void free_shared_memory()
        {
            if (_sharedMemory)
            {
                _sharedMemory->~shared_memory();
            }

            bi::shared_memory_object::remove(_sharedName.data());
        }

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

            _sharedMemory->status.startProducing = true;

            readFromFile(inFile);

            _sharedMemory->status.endProducing = true;

            while (_sharedMemory->status.endConsuming == false)
            {
                // Waiting for ending consumer process...
            }

            my::log::producer_logger()->info("[Producer] Producing done!");
        }

        /// @brief Reads data from file and saves to shared memory
        inline void readFromFile(std::ifstream &inFile)
        {
            while (inFile)
            {
                for (auto &curBuf : _sharedMemory->buffer)
                {
                    if (curBuf.readSize == 0)
                    {
                        inFile.read(curBuf.data, curBuf.bufferSize);

                        curBuf.readSize = inFile.gcount();
                    }
                }
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

            while (_sharedMemory->status.startProducing == false)
            {
                // Waiting for staring producer process...
            }

            writeToFile(outFile);

            _sharedMemory->status.endConsuming = true;

            my::log::consumer_logger()->info("[Consumer] Consuming done!");
        }

        /// @brief Reads data from shared memory and saves to file
        inline void writeToFile(std::ofstream &outFile)
        {
            auto writeFromBuffer = [&](bool &write)
            {
                for (auto &curBuf : _sharedMemory->buffer)
                {
                    if (curBuf.readSize == 0)
                    {
                        continue;
                    }

                    outFile.write(curBuf.data, curBuf.readSize);

                    if (curBuf.readSize < buffer::bufferSize)
                    {
                        write = false;
                    }

                    curBuf.readSize = 0;
                }
            };

            bool write{true};
            while (write)
            {
                writeFromBuffer(write);

                if (_sharedMemory->status.endProducing)
                {
                    write = false;
                }
            }

            writeFromBuffer(write);
        }

    private:
        std::string_view _inFilepath;
        std::string_view _outFilepath;
        std::string_view _sharedName;

        shm::shared_memory *_sharedMemory{nullptr};
        bool _isProducer{false};
    };

} // namespace inter

#endif // __INCLUDE_COPY_FILE_HPP__