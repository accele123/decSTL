// modified on 2023/4/23 MON
// author: accele123
// vector 的简单实现

#ifndef _NEW_VECTOR_
#define _NEW_VECTOR_

#include <iostream>
#include "alloc.h"
#include <cstddef>
#include <cstdlib>

/// @brief vector的反向迭代器
template <class T>
class vec_reiterator
{
private:
    T *ptr;

public:
    vec_reiterator(): ptr(0) {}
    vec_reiterator(T *p) : ptr(p) {}
    vec_reiterator(const vec_reiterator &p) : ptr(p.ptr) {}
    vec_reiterator operator=(const vec_reiterator &p)
    {
        if (p.ptr == ptr)
            return *this;
        ptr = p.ptr;
        return *this;
    }
    bool operator==(const vec_reiterator &p) const { return ptr == p.ptr; }
    bool operator!=(const vec_reiterator &p) const { return ptr != p.ptr; }
    T operator*() { return *ptr; }
    T *operator->() { return &(operator*()); }
    void operator+=(const size_t n) { ptr -= n; }
    void operator-=(const size_t n) { ptr += n; }
    vec_reiterator operator+(const size_t n) { return vec_reiterator(ptr - n); }
    vec_reiterator operator-(const size_t n) { return vec_reiterator(ptr + n); }
    ptrdiff_t operator-(const vec_reiterator &p) { return (ptrdiff_t)(p.ptr - ptr); }
    vec_reiterator operator++()
    {
        --ptr;
        return *this;
    }
    vec_reiterator operator--()
    {
        ++ptr;
        return *this;
    }
    vec_reiterator operator++(int)
    {
        vec_reiterator temp = *this;
        --ptr;
        return temp;
    }
    vec_reiterator operator--(int)
    {
        vec_reiterator temp = *this;
        ++ptr;
        return temp;
    }
};

/******************************/

/// @brief vector实现
/// @tparam T 类型
/// @tparam Alloc 分配器类型
template <class T, class Alloc = sim_alloc<T>>
class vector
{
public:
    /******************************/
    // 接口
    using type = T;
    using pointer = T *;
    using con_pointer = const T *;
    using refer = T &;
    using con_refer = const T &;
    using iterator = pointer;
    using con_iterator = const T *;
    using reiterator = vec_reiterator<T>;
    /******************************/
    // 函数实现
    vector() : _begin(nullptr), _end(nullptr), _end_of_mem(nullptr) {}
    vector(const T &val)
    {
        _begin = vec_alloc(1);
        _end = _end_of_mem = _begin + 1;
        construct(_begin, val);
    }
    vector(const size_t n, const T &val)
    {
        _begin = vec_alloc(n);
        _end = _end_of_mem = _begin + n;
        for (size_t i = 0; i < n; i++)
            construct((_begin + i), val);
    }
    vector(const std::initializer_list<T> &p)
    {
        _begin = vec_alloc(p.size());
        size_t j = 0;
        for (auto i = p.begin(); i != p.end(); i++, j++)
            construct(_begin + j, *i);
        _end = _end_of_mem = _begin + p.size();
    }
    vector(const vector &p)
    {
        size_t n = p.size();
        _begin = vec_alloc(n);
        _end = _end_of_mem = _begin + n;
        for (size_t i = 0; i < n; i++)
            construct((_begin + i), *(p.begin() + i));
    }
    vector &operator=(const vector &p)
    {
        if (p.begin() == begin())
            return *this;
        clear();
        size_t n = p.size();
        if (n > capacity())
        {
            vec_dealloc(_begin);
            _begin = vec_alloc(n);
            _end_of_mem = _begin + n;
        }
        for (size_t i = 0; i < n; i++)
            construct((_begin + i), *(p.begin() + i));
        _end = _begin + n;
        return *this;
    }
    vector &operator=(const std::initializer_list<T> &p)
    {
        clear();
        size_t n = p.size();
        if (n > capacity())
        {
            _begin = vec_alloc(n);
            _end_of_mem = _begin + n;
        }
        size_t j = 0;
        for (auto i = p.begin(); i != p.end(); i++, j++)
            construct(_begin + j, *i);
        _end = _begin + n;
        return *this;
    }
    ~vector()
    {
        clear();
        if (empty())
            vec_dealloc(_begin);
    }
    size_t size() const { return (_end - _begin); }
    size_t capacity() const { return _end_of_mem - _begin; }
    size_t max_size() const { return (size_t)(-1); }
    iterator begin() const { return _begin; }
    iterator end() const { return _end; }
    refer back() const { return *(_end - 1); }
    refer at(const size_t n) const { return *(_begin + n); }
    con_iterator cbegin() const { return _begin; }
    con_iterator cend() const { return _end; }
    reiterator rbegin() const { return reiterator(_end - 1); }
    reiterator rend() const { return reiterator(_begin - 1); }
    refer operator[](const size_t n) const { return *(_begin + n); }
    bool empty() const { return _begin == _end; }
    void push_back(const T &val)
    {
        if (_end_of_mem != _end)
        {
            construct(_end, val);
            ++_end;
        }
        else
        {
            size_t len = size();
            pointer temp = vec_alloc(len == 0 ? 1 : 2 * len);
            for (size_t i = 0; i < len; i++)
                construct((temp + i), *(_begin + i));
            _begin = temp;
            _end = _begin + len;
            _end_of_mem = _begin + 2 * len;
            construct(_end, val);
            ++_end;
        }
    }
    void pop_back()
    {
        if (empty())
        {
            std::cout << "Error! vector is empty" << std::endl;
            return;
        }
        destroy(--_end);
    }
    void clear()
    {
        if (!empty())
        {
            for (auto i = _end - 1; i != _begin; i--)
                destroy(i);
            destroy(_begin);
        }
    }
    /******************************/
private:
    /******************************/
    // 内部实际元素
    /// @brief 起始地址
    pointer _begin;
    /// @brief 元素终止地址的下一位
    pointer _end;
    /// @brief 内存终止地址的下一位
    pointer _end_of_mem;
    /******************************/
    // 内部操作函数实现
    /// @brief 分配内存
    /// @param n 分配元素数目
    /// @return 分配内存的地址
    pointer vec_alloc(const size_t n) { return Alloc::allocate(n); }
    /// @brief 释放内存
    /// @param p 释放元素首地址
    void vec_dealloc(pointer p) { Alloc::deallocate(p); }
    /// @brief 构造对象
    /// @param p 构造目标的地址
    /// @param val 构造值
    void construct(pointer p, const T &val) { Alloc::construct(p, val); }
    /// @brief 析构对象
    /// @param p 析构目标的地址
    void destroy(pointer p) { Alloc::destroy(p); }
    /******************************/
};

#endif