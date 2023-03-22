#pragma once
#ifndef __INCLUDE_SHARED_MEMORY_HPP__
#define __INCLUDE_SHARED_MEMORY_HPP__

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "buffer.hpp"

namespace shm
{

    namespace bi = boost::interprocess;

    static const char MutexName[] = {"COPY_FILE_MUTEX"};
    static const char BufferName[] = {"COPY_FILE_BUFFER"};
    static const char RemainedSymbolsName[] = {"COPY_FILE_REMAINED_SYMBOLS"};
    static const char CondVarName[] = {"COPY_FILE_COND_VAR"};
    static const char StartProducerName[] = {"COPY_FILE_START_PRODUCER"};
    static const char EndConsumerName[] = {"COPY_FILE_END_CONSUMER"};

    static inline void remove()
    {
        bi::shared_memory_object::remove(RemainedSymbolsName);
        bi::shared_memory_object::remove(BufferName);
        bi::shared_memory_object::remove(StartProducerName);
        bi::shared_memory_object::remove(EndConsumerName);
        bi::named_mutex::remove(MutexName);
        bi::named_condition::remove(CondVarName);
    }

    // --- GETTERS ---

    static inline bool *getStartProducer()
    {
        static bi::shared_memory_object sharedMemmory{bi::open_or_create,
                                                      shm::StartProducerName,
                                                      bi::read_write};

        sharedMemmory.truncate(sizeof(bool));

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        auto ptr = static_cast<bool *>(region.get_address());
        *ptr = false;

        return ptr;
    }

    static inline bool *getEndConsumer()
    {
        static bi::shared_memory_object sharedMemmory{bi::open_or_create,
                                                      shm::EndConsumerName,
                                                      bi::read_write};

        sharedMemmory.truncate(sizeof(bool));

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        auto ptr = static_cast<bool *>(region.get_address());
        *ptr = false;

        return ptr;
    }

    static inline std::size_t *getRemainedSymbols()
    {
        static bi::shared_memory_object sharedMemmory{bi::open_only,
                                                      shm::RemainedSymbolsName,
                                                      bi::read_write};

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        return static_cast<std::size_t *>(region.get_address());
    }

    static inline inter::buffer *getBuffer()
    {
        static bi::shared_memory_object sharedMemmory{bi::open_only,
                                                      shm::BufferName,
                                                      bi::read_write};

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        return static_cast<inter::buffer *>(region.get_address());
    }

    // --- SETTERS ---

    static inline std::size_t *setRemainedSymbols(std::size_t value)
    {
        static bi::shared_memory_object sharedMemmory{bi::create_only,
                                                      shm::RemainedSymbolsName,
                                                      bi::read_write};

        sharedMemmory.truncate(sizeof(std::size_t));

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        return new (region.get_address()) std::size_t(value);
    }

    static inline inter::buffer *setBuffer()
    {
        static bi::shared_memory_object sharedMemmory{bi::create_only,
                                                      shm::BufferName,
                                                      bi::read_write};

        sharedMemmory.truncate(sizeof(inter::buffer));

        static bi::mapped_region region(sharedMemmory, bi::read_write);

        return new (region.get_address()) inter::buffer();
    }

} // namespace shm

#endif // __INCLUDE_SHARED_MEMORY_HPP__