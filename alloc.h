// modified on 2023/4/21 FRI
// author: accele123
// 复杂分配器的实现

#ifndef _NEW_COMPLEX_ALLOC
#define _NEW_COMPLEX_ALLOC

#include <iostream>
#include <cstdlib>
#include <cstddef>
#include "alloc_tools.h"

class alloc_lev_fir;
class alloc_lev_sec;

/******************************/
// 选择内存分配失败后合适的抛出机制
#if 0
#define _THROW_ALLOC_ERROR throw bad_alloc;
#elif !#define(_THROW_ALLOC_ERROR_)
#define _THROW_ALLOC_ERROR_                                 \
    std::cerr << " Error! Alloc is failure! " << std::endl; \
    exit(1);
#endif

/// @brief 选择合适的分配器，alloc_choose == 1 选择一级分配器,alloc_choose == 2 选择二级分配器;
#define alloc_choose 2
// #define alloc_choose 1
#if alloc_choose == 1
#define alloc alloc_lev_fir
#elif alloc_choose == 2
#define alloc alloc_lev_sec
#endif

/******************************/
/// @brief 外包装分配器，仅仅包装了另一个复杂分配器的接口
/// @tparam T 分配类型
/// @tparam Alloc 分配器，默认使用alloc，即上文中宏定义的分配器作为内部实现
template <class T, class Alloc = alloc>
class sim_alloc
{
public:
    using type = T;
    using pointer = T *;
    using refer = T &;
    static pointer allocate(const size_t n) { return (pointer)Alloc::allocate(n * sizeof(T)); }
    static pointer allocate() { return Alloc::allocate(sizeof(T)); }
    static void deallocate(pointer p) { Alloc::deallocate(p, 1); }
    static void deallocate(pointer p, const size_t n) { Alloc::deallocate(p, n); }
    size_t max_size() const { return (size_t)(-1); }
    static void construct(pointer p, const T &val) { _construct(p, val); }
    static void destroy(pointer p) { _destroy(p); }
};
/******************************/

/// @brief 一级分配器，直接使用malloc和free等来处理内存分配问题
class alloc_lev_fir
{
private:
    /// @brief 处理alloc失败的函数指针，默认为0，可以被set_own_handler更改
    static void (*alloc_error_handler)();

public:
    /// @brief 更改变量alloc_error_handler
    /// @param f 自定义新的alloc_error_handler指针的值
    /// @return 原来的alloc_error_handler指针
    static void (*set_own_handler(void (*f)()))()
    {
        void (*ret)() = alloc_error_handler;
        alloc_error_handler = f;
        return ret;
    }
    static void *allocate(size_t n)
    {
        void *ret = malloc(n);
        if (ret == 0)
            error_allocate(n);
        return ret;
    }
    static void deallocate(void *p) { free(p); }
    static void *error_allocate(size_t n)
    {
        void (*fun)() = alloc_error_handler;
        void *ret = 0;
        while (1)
        {
            if (fun == 0)
            {
                _THROW_ALLOC_ERROR_
            }
            else
            {
                ret = malloc(n);
                if (ret)
                    return ret;
            }
        }
    }
};
void (*alloc_lev_fir::alloc_error_handler)() = 0;

/******************************/
// 二级分配器所需要的宏定义
#define ALLOC_MIN_SIZE 8
#define ALLOC_MAX_SIZE 128
#define ALLOC_LEN 16
#define ALLOC_OBJ_NUM 20

/// @brief 二级分配器，维护一个内存池来分配内存，内存池的最大分配额度设置为128字节，超出将会使用一级分配器来分配内存
class alloc_lev_sec
{
private:
    using malloc_alloc = alloc_lev_fir;

public:
    union obj
    {
        obj *next; // 维护一个链表，但是使用union实现复用，节省内存空间
        char node_val[1];
    };
    static void *allocate(const size_t n)
    {
        obj *ret = 0, **list = 0;
        if (n > ALLOC_MAX_SIZE)
            return malloc_alloc::allocate(n);
        list = free_list + index(n);
        ret = *list;
        if (ret == 0)
            return refill(up_to_minsize(n));
        *list = ret->next;
        return ret;
    }
    static void deallocate(void *p, size_t n)
    {
        if (n > ALLOC_MAX_SIZE)
        {
            malloc_alloc::deallocate(p);
            return;
        }
        obj **list = free_list + index(n);
        ((obj *)(p))->next = *list;
        *list = ((obj *)(p));
    }
    static void *refill(const size_t n)
    {
        size_t nobj = ALLOC_OBJ_NUM;
        obj *ret = 0, **list = 0;
        obj *chunk = (obj *)chunk_alloc(n, nobj);
        ret = chunk;
        if (nobj == 1)
            return ret;
        ret = chunk;
        list = free_list + index(n);
        *list = (obj *)((char *)chunk + n);
        obj *next_obj = *list;
        obj *cur_obj = 0;
        for (int i = 1; i < nobj - 1; i++)
        {
            cur_obj = next_obj;
            next_obj = (obj *)((char *)cur_obj + n);
            cur_obj->next = next_obj;
        }
        next_obj->next = 0;
        return ret;
    }
    static void *chunk_alloc(const size_t n, size_t &nobj)
    {
        size_t sum = n * nobj;
        size_t left = list_end - list_begin;
        char *ret = 0;
        if (left >= sum)
        {
            ret = list_begin;
            list_begin += sum;
            return ret;
        }
        else if (left >= n)
        {
            nobj = left / n;
            sum = n * nobj;
            ret = list_begin;
            list_begin += sum;
            return ret;
        }
        else
        {
            size_t need = 2 * sum + up_to_minsize(heap_size >> 4);
            if (left > 0)
            {
                obj **list = free_list + index(n);
                ((obj *)(list_begin))->next = *list;
                *list = ((obj *)(list_begin));
            }
            list_begin = (char *)malloc(need);
            if (list_begin == 0)
            {
                obj **list = 0, *p = 0;
                for (int i = n; i <= ALLOC_MAX_SIZE; i += ALLOC_MIN_SIZE)
                {
                    list = free_list + index(i);
                    p = *list;
                    if (p != 0)
                    {
                        *list = p->next;
                        list_begin = (char *)p;
                        list_end = list_begin + i;
                        return chunk_alloc(n, nobj);
                    }
                }
            }
            list_end = 0;
            list_begin = (char *)malloc_alloc::allocate(need);
            heap_size = need;
            list_end = list_begin + need;
            return chunk_alloc(n, nobj);
        }
    }

private:
    static obj *free_list[ALLOC_LEN];
    /// @brief 内存池的起始点
    static char *list_begin;
    /// @brief 内存池的终止点
    static char *list_end;
    /// @brief 动态调节内存池的大小
    static size_t heap_size;
    static size_t index(const size_t n) { return (n + ALLOC_MIN_SIZE - 1) / (ALLOC_MIN_SIZE) - 1; }
    static size_t up_to_minsize(const size_t n) { return (n + ALLOC_MIN_SIZE - 1) & (~ALLOC_MIN_SIZE - 1); }
};
alloc_lev_sec::obj *alloc_lev_sec::free_list[ALLOC_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char *alloc_lev_sec::list_begin = 0;
char *alloc_lev_sec::list_end = 0;
size_t alloc_lev_sec::heap_size = 0;
/******************************/

#endif
