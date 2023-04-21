// modified on 2023/4/21 FRI
// author: accele123
// 分配器搭配全局工具的实现

#ifndef _NEW_ALLOC_TOOLS_
#define _NEW_ALLOC_TOOLS_
#include <cstdlib>
#include <cstddef>

template <class T1, class T2>
void _construct(T1 *p, const T2 &val)
{
    new (p) T1(val);
}
template <class T>
void _destroy(T *p)
{
    p->~T();
}

#endif