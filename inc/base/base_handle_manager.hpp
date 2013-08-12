/********************************************************
 * Description : manager of unique identification
 * Data        : 2013-08-10 20:08:15
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_HANDLE_MANAGER_H
#define COMMON_BASE_HANDLE_MANAGER_H


#include <deque>
#include <algorithm>
#include <functional>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_locker.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

template <typename HandleType, typename LockerType = NullLocker>
class YRK_EXPORT_DLL HandleManager : public BaseUncopy
{
public:
    HandleManager(HandleType min, HandleType max);

public:
    bool acquire(HandleType & handle);
    bool release(HandleType handle);

private:
    const HandleType         m_min_handle;
    const HandleType         m_max_handle;
    HandleType               m_next_handle;
    std::deque<HandleType>   m_unused_handle;
    LockerType               m_locker;
};

template <typename HandleType, typename LockerType>
HandleManager<HandleType, LockerType>::HandleManager
(
    HandleType min, HandleType max
)
    : m_min_handle(min)
    , m_max_handle(max)
    , m_next_handle(min)
    , m_unused_handle()
    , m_locker()
{

}

template <typename HandleType, typename LockerType>
bool HandleManager<HandleType, LockerType>::acquire(HandleType & handle)
{
    BaseGuard<LockerType> guard(m_locker);

    if (!m_unused_handle.empty())
    {
        handle = m_unused_handle.back();
        m_unused_handle.pop_back();
        return(true);
    }

    if (m_next_handle >= m_max_handle)
    {
        return(false);
    }

    handle = m_next_handle++;

    return(true);
}

template <typename HandleType, typename LockerType>
bool HandleManager<HandleType, LockerType>::release(HandleType handle)
{
    BaseGuard<LockerType> guard(m_locker);

    if (handle <  m_min_handle || 
        handle >= m_next_handle)
    {
        return(false);
    }

    if (m_unused_handle.end() != 
        std::find(m_unused_handle.begin(), m_unused_handle.end(), handle))
    {
        return(false);
    }

    if (handle + 1 == m_next_handle)
    {
        m_next_handle = handle;
    }
    else
    {
        m_unused_handle.push_back(handle);
        std::sort(m_unused_handle.begin(), m_unused_handle.end(), 
                  std::greater<HandleType>());
    }

    typename std::deque<HandleType>::iterator iter = m_unused_handle.begin();
    while (m_unused_handle.end() != iter)
    {
        if (*iter + 1 == m_next_handle)
        {
            m_next_handle = *iter++;
        }
        else
        {
            break;
        }
    }
    m_unused_handle.erase(m_unused_handle.begin(), iter);

    return(true);
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_HANDLE_MANAGER_H
