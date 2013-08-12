/********************************************************
 * Description : resource guard class
 * Data        : 2013-05-20 12:48:51
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_GUARD_H
#define COMMON_BASE_GUARD_H


#include "base_common.h"
#include "base_uncopy.h"

NAMESPACE_COMMON_BEGIN

template <typename ResourceType>
class BaseGuard : private BaseUncopy
{
public:
    BaseGuard(ResourceType & resource);
    ~BaseGuard();

public:
    void acquire();
    void release();

private:
    bool           m_owned;
    ResourceType & m_resource;
};

template <typename ResourceType>
BaseGuard<ResourceType>::BaseGuard(ResourceType & resource)
    : m_owned(false), m_resource(resource)
{
    acquire();
}

template <typename ResourceType>
BaseGuard<ResourceType>::~BaseGuard()
{
    if (m_owned)
    {
        release();
    }
}

template <typename ResourceType>
void BaseGuard<ResourceType>::acquire()
{
    m_resource.acquire();
    m_owned = true;
}

template <typename ResourceType>
void BaseGuard<ResourceType>::release()
{
    m_resource.release();
    m_owned = false;
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_GUARD_H
