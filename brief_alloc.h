// modified on 2023/4/21 FRI
// author: accele123
// 极简分配器的实现

#ifndef _NEW_BRIEF_ALLOC_
#define _NEW_BRIEF_ALLOC_

#include <new>
#include <cstddef>
#include <cstdlib>
#include <iostream>

/// @brief 极简分配器,使用::operator new 和 ::operator delete 来处理内存
/// @tparam T 元素分配类型
template <class T>
class alloc1
{
public:
    /******************************/
    // 接口
    using type = T;
    using pointer = T *;
    using con_pointer = const T *;
    using refer = T &;
    using con_refer = const T &;
    template <class U>
    struct rebind
    {
        using other = alloc1<U>;
    };
    /******************************/
    // 函数
    static pointer allocate(size_t n)
    {
        T *ret = (T *)(::operator new(sizeof(type) * size_t(n)));
        return ret;
    }
    static void deallocate(pointer p)
    {
        ::operator delete(p);
    }
    static void construct(pointer p, const T &val)
    {
        new (p) T(val);
    }
    static void destroy(pointer p)
    {
        p->~T();
    }
    static pointer address(refer p) 
    {
        return (pointer)&p;
    }
    static con_pointer const_address(con_refer p) 
    {
        return (con_pointer)&p;
    }
    static size_t max_size() 
    {
        return (size_t)(-1);
    }
    /******************************/
};

#endif