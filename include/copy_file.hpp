#pragma once
#ifndef __INCLUDE_COPY_FILE_HPP__
#define __INCLUDE_COPY_FILE_HPP__

#include <fstream>

#include "exception.hpp"
#include "logger.hpp"
#include "shared_memory.hpp"

namespace inter
{
    namespace bi = boost::interprocess;

    namespace detail
    {
        /// @brief Static termination data
        static struct
        {
            shm::shared_memory **smnPtr{nullptr};
            std::string_view *name{nullptr};
            bool *isProducer{nullptr};
        } termination_struct;

        /// @brief Function to handle termination. I don't check pointers, 
        ///        because I believe that user set them
        static void termination()
        {
            my::log::deflogger()->info("Termination call!!!");
            my::log::deflogger()->flush();

            (*termination_struct.smnPtr)->status.endConsuming = true;
            (*termination_struct.smnPtr)->status.endProducing = true;

            if (*termination_struct.isProducer)
            {
                bi::shared_memory_object::remove(termination_struct.name->data());
            }

            std::abort();
        }
    } // namespace detail

    class copy_file
    {
    public:
        copy_file(std::string_view in_filepath, std::string_view out_filepath,
                  std::string_view shared_name)
            : _inFilepath{in_filepath}, _outFilepath{out_filepath},
              _sharedName{shared_name}
        {
            my::log::deflogger()->info("The copy_file constructed!");
            my::log::deflogger()->flush();
        }

        ~copy_file()
        {
            my::log::deflogger()->info("The copy_file destructed!");
            my::log::deflogger()->flush();
            
            if (_isProducer)
            {
                my::log::producer_logger()->info("The copy_file destructed!");

                _sharedMemory->status.endProducing = true;

                free_shared_memory();
            }
            else
            {
                my::log::consumer_logger()->info("The copy_file destructed!");

                _sharedMemory->status.endConsuming = true;
            }
        }

        /// @brief Gets pointers from shared memory and runs function that read-write
        ///        data form-to file throught shared memory
        inline void run()
        {
            // Sets termination params
            detail::termination_struct.smnPtr = &_sharedMemory;
            detail::termination_struct.isProducer = &_isProducer;
            detail::termination_struct.name = &_sharedName;
            std::set_terminate(detail::termination);

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
                my::log::producer_logger()->error("Cannot run copping"
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

            my::log::producer_logger()->info("Producing done!");
        }

        /// @brief Reads data from file and saves to shared memory
        inline void readFromFile(std::ifstream &inFile)
        {
            while (inFile)
            {
                for (auto &curBuf : _sharedMemory->buffer)
                {
                    if (_sharedMemory->status.endConsuming)
                    {
                        my::log::producer_logger()->warn("Maybe some error!");
                        return;
                    }

                    if (curBuf.readSize == 0)
                    {
                        inFile.read(curBuf.data, curBuf.bufferSize);

                        curBuf.readSize = inFile.gcount();

                        throw shm::exception("Error with read from file"); // Error to test
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
                my::log::consumer_logger()->error("Cannot run copping"
                                                  " because output filepath is invalid!");
                return;
            }

            while (_sharedMemory->status.startProducing == false)
            {
                // Waiting for staring producer process...
            }

            writeToFile(outFile);

            _sharedMemory->status.endConsuming = true;

            my::log::consumer_logger()->info("Consuming done!");
        }

        /// @brief Reads data from shared memory and saves to file
        inline void writeToFile(std::ofstream &outFile)
        {
            auto writeFromBuffer = [&]()
            {
                for (auto &curBuf : _sharedMemory->buffer)
                {
                    if (curBuf.readSize == 0)
                    {
                        continue;
                    }

                    outFile.write(curBuf.data, curBuf.readSize);

                    curBuf.readSize = 0;

                    // throw shm::exception("Error with write to file"); // Error to test
                }
            };

            bool write{true};
            while (write)
            {
                writeFromBuffer();

                if (_sharedMemory->status.endProducing)
                {
                    write = false;
                }
            }

            writeFromBuffer();
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