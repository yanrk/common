/********************************************************
 * Description : object pool class template
 * Data        : 2013-05-23 18:27:48
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_OBJECT_POOL_H
#define COMMON_BASE_OBJECT_POOL_H


#include <string>
#include <list>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_exception.h"
#include "base_memory.h"
#include "base_locker.h"
#include "base_guard.hpp"

NAMESPACE_COMMON_BEGIN

template <typename ObjectType, typename LockerType = NullLocker>
class YRK_EXPORT_DLL ObjectPool : private BaseUncopy
{
public:
    ObjectPool
    (
        const char * pool_name = nullptr, 
        const char * lock_name = nullptr
    );

    ~ObjectPool();

public:
    ObjectType * acquire();
    void release(ObjectType *& obj);

public:
    void clear();

public:
    size_t size();

public:
    void operator << (std::list<ObjectType *> & obj_list);
    void operator >> (std::list<ObjectType *> & obj_list);

    void operator << (ObjectPool & other);
    void operator >> (ObjectPool & other);

private:
    std::string             m_name;
    std::list<ObjectType *> m_objs;
    LockerType              m_locker;
};

template <typename ObjectType, typename LockerType>
ObjectPool<ObjectType, LockerType>::ObjectPool
(
    const char * pool_name, 
    const char * lock_name
)
    : m_name(nullptr != pool_name ? pool_name : "")
    , m_objs()
    , m_locker(nullptr != lock_name ? lock_name : "")
{

}

template <typename ObjectType, typename LockerType>
ObjectPool<ObjectType, LockerType>::~ObjectPool()
{
    clear();
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::clear()
{
    BaseGuard<LockerType> pool_guard(m_locker);
    typename std::list<ObjectType *>::iterator iter = m_objs.begin();
    if (m_objs.end() != iter)
    {
        BASE_DELETE(*iter);
        ++iter;
    }
    m_objs.clear();
}

template <typename ObjectType, typename LockerType>
ObjectType * ObjectPool<ObjectType, LockerType>::acquire()
{
    BaseGuard<LockerType> pool_guard(m_locker);
    ObjectType * obj = nullptr;
    if (m_objs.empty())
    {
        BASE_NEW(obj, ObjectType);
    }
    else
    {
        obj = m_objs.front();
        m_objs.pop_front();
    }
    return obj;
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::release(ObjectType *& obj)
{
    if (nullptr != obj)
    {
        BaseGuard<LockerType> pool_guard(m_locker);
        m_objs.push_back(obj);
        obj = nullptr;
    }
}

template <typename ObjectType, typename LockerType>
size_t ObjectPool<ObjectType, LockerType>::size()
{
    BaseGuard<LockerType> pool_guard(m_locker);
    return m_objs.size();
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::operator << 
(
    std::list<ObjectType *> & obj_list
)
{
    if (obj_list != m_objs)
    {
        BaseGuard<LockerType> pool_guard(m_locker);
        m_objs.splice(m_objs.end(), obj_list);
    }
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::operator >> 
(
    std::list<ObjectType *> & obj_list
)
{
    if (obj_list != m_objs)
    {
        BaseGuard<LockerType> pool_guard(m_locker);
        obj_list.splice(obj_list.end(), m_objs);
    }
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::operator << 
(
    ObjectPool<ObjectType, LockerType> & other
)
{
    other >> m_objs;
}

template <typename ObjectType, typename LockerType>
void ObjectPool<ObjectType, LockerType>::operator >> 
(
    ObjectPool<ObjectType, LockerType> & other
)
{
    other << *this;
}

template <typename ObjectType, typename LockerType>
void operator << 
(
    std::list<ObjectType *> & obj_list, 
    ObjectPool<ObjectType, LockerType> & obj_pool
)
{
    obj_pool >> obj_list;
}

template <typename ObjectType, typename LockerType>
void operator >> 
(
    std::list<ObjectType *> & obj_list, 
    ObjectPool<ObjectType, LockerType> & obj_pool
)
{
    obj_pool << obj_list;
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_OBJECT_POOL_H
