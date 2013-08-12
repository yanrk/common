/********************************************************
 * Description : memory pool class template
 * Data        : 2013-06-18 11:47:56
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_MEMORY_POOL_H
#define COMMON_BASE_MEMORY_POOL_H


#include <cassert>
#include <vector>
#include <list>

#include "base_macro.h"
#include "base_uncopy.h"
#include "base_memory.h"
#include "base_locker.h"
#include "base_guard.hpp"
#include "base_exception.h"
#include "base_log.h"

NAMESPACE_COMMON_BEGIN

struct YRK_EXPORT_DLL MEMORY_NODE : private BaseUncopy
{
    const int index;
    const int capacity;
    int       size;
    char      buffer[1];

    MEMORY_NODE(int idx, int cap)
        : index(idx), capacity(cap), size(0)
    {

    }

    ~MEMORY_NODE()
    {

    }
};

static const int DEFAULT_MEMORY_BLOCK_KINDS = 14;
static const int DEFAULT_MEMORY_BLOCK_MIN_SIZE = 8;

template <typename LockerType = NullLocker>
class YRK_EXPORT_DLL MemoryPool : private BaseUncopy
{
public:
    MemoryPool(int memory_block_kinds = DEFAULT_MEMORY_BLOCK_KINDS, 
               int memory_block_min_size = DEFAULT_MEMORY_BLOCK_MIN_SIZE);
    ~MemoryPool();

public:
    void clear();

public:
    MEMORY_NODE * acquire(int size);
    void release(MEMORY_NODE *& memory_node);

private:
    struct MemoryList
    {
        std::list<MEMORY_NODE *>  m_list;
        LockerType                m_locker;
    };

    const int                     m_block_kinds;
    const int                     m_block_min_size;
    MemoryList                  * m_memory;
};

static int get_1_of_binary(int value)
{
    int count = 0;
    while (0 != value)
    {
        value &= (value - 1);
        ++count;
    }
    return(count);
}

static int get_memory_index(int adjusted_size, int memory_block_min_size)
{
    assert(adjusted_size >= memory_block_min_size);

    adjusted_size /= memory_block_min_size * 2;

    int index = 0;
    while (0 != adjusted_size)
    {
        adjusted_size >>= 1;
        ++index;
    }

    return(index);
}

template <typename LockerType>
MemoryPool<LockerType>::MemoryPool
(
    int memory_block_kinds, 
    int memory_block_min_size
)
    : m_block_kinds(memory_block_kinds)
    , m_block_min_size(memory_block_min_size)
    , m_memory(nullptr)
{
    assert(1 == get_1_of_binary(m_block_min_size));
    BASE_NEW_A(m_memory, MemoryList, memory_block_kinds);
    if (nullptr == m_memory)
    {
        RUN_LOG_DBG("create MemoryList failed");
        assert(nullptr == m_memory);
        BASE_THROW("create MemoryList failed");
    }
}

template <typename LockerType>
MemoryPool<LockerType>::~MemoryPool()
{
    clear();
    BASE_DELETE_A(m_memory);
}

template <typename LockerType>
void MemoryPool<LockerType>::clear()
{
    for (int index = 0; index < m_block_kinds; ++index)
    {
        MemoryList & memory = m_memory[index];
        BaseGuard<LockerType> memory_guard(memory.m_locker);
        std::list<MEMORY_NODE *> & memory_node_list = memory.m_list;
        std::list<MEMORY_NODE *>::iterator iter = memory_node_list.begin();
        while (memory_node_list.end() != iter)
        {
            MEMORY_NODE * memory_node = *iter;
            memory_node->~MEMORY_NODE();
            ++iter;
            char * buffer = reinterpret_cast<char *>(memory_node);
            BASE_DELETE_A(buffer);
        }
        memory_node_list.clear();
        memory_guard.release();
    }
}

template <typename LockerType>
MEMORY_NODE * MemoryPool<LockerType>::acquire(int size)
{
    size += sizeof(MEMORY_NODE);

    int adjust_size = m_block_min_size;
    while (size > m_block_min_size)
    {
        size += 1;
        size >>= 1;
        adjust_size <<= 1;
    }

    int index = get_memory_index(adjust_size, m_block_min_size);

    if (m_block_kinds > index)
    {
        MemoryList & memory = m_memory[index];
        BaseGuard<LockerType> memory_guard(memory.m_locker);
        std::list<MEMORY_NODE *> & memory_node_list = memory.m_list;
        if (!memory_node_list.empty())
        {
            MEMORY_NODE * memory_node = memory_node_list.front();
            memory_node_list.pop_front();
            return(memory_node);
        }
        memory_guard.release();
    }

    MEMORY_NODE * memory_node = nullptr;
    char * buffer = nullptr;
    BASE_NEW_A(buffer, char, adjust_size);
    if (nullptr != buffer)
    {
        new (buffer) MEMORY_NODE(index, adjust_size);
        memory_node = reinterpret_cast<MEMORY_NODE *>(buffer);
    }
    return(memory_node);
}

template <typename LockerType>
void MemoryPool<LockerType>::release(MEMORY_NODE *& memory_node)
{
    if (nullptr == memory_node)
    {
        return;
    }

    memory_node->size = 0;

    if (m_block_kinds <= memory_node->index)
    {
        memory_node->~MEMORY_NODE();
        char * buffer = reinterpret_cast<char *>(memory_node);
        BASE_DELETE_A(buffer);
        memory_node = nullptr;
        return;
    }

    MemoryList & memory = m_memory[memory_node->index];
    BaseGuard<LockerType> memory_guard(memory.m_locker);
    std::list<MEMORY_NODE *> & memory_node_list = memory.m_list;
    memory_node_list.push_back(memory_node);
    memory_guard.release();
    memory_node = nullptr;
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_MEMORY_POOL_H
