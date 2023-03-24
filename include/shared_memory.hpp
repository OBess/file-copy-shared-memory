#pragma once
#ifndef __INCLUDE_SHARED_MEMORY_HPP__
#define __INCLUDE_SHARED_MEMORY_HPP__

#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "buffer.hpp"
#include "status.hpp"

namespace shm
{

    namespace bi = boost::interprocess;

    struct shared_memory
    {
        inter::status status{};
        bi::interprocess_mutex mutex{};

        inter::buffer buffer[2]{};
        bool switcher{false};
    };

    /// @brief Checks, if shared memory with name exists
    /// @param name name of shared memory
    /// @return True, if shared memory with name exists, False otherwise
    inline bool exists(std::string_view name)
    {
        try
        {
            [[maybe_unused]] const bi::shared_memory_object shm_source(bi::create_only,
                                                                       name.data(),
                                                                       bi::read_only);
            return false;
        }
        catch (const std::exception &ex)
        {
            return true;
        }

        return false;
    }

    /// @brief Opens or creates shared memory and returns poninter to it
    /// @param name name of shared memory
    /// @return Pointer to shared memory
    static inline uint8_t *getShareMemory(std::string_view name)
    {
        static bi::shared_memory_object sharedMemmory{bi::open_or_create,
                                                      name.data(),
                                                      bi::read_write};

        static constexpr std::size_t shareMemorySize{sizeof(shared_memory)};

        if (int64_t currentSize{0}; sharedMemmory.get_size(currentSize))
        {
            if (currentSize < shareMemorySize)
            {
                sharedMemmory.truncate(shareMemorySize);
            }
        }

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        if (inter::status *status = reinterpret_cast<inter::status *>(region.get_address());
            status->startProducing == false)
        {
            std::memset(region.get_address(), 0, shareMemorySize);
        }

        return reinterpret_cast<uint8_t *>(region.get_address());
    }

} // namespace shm

#endif // __INCLUDE_SHARED_MEMORY_HPP__