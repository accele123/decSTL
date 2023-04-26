// modified on 2023/4/26 WEN
// author: accele123
// list 的简单实现

#ifndef _NEW_LIST_
#define _NEW_LIST_

#include <iostream>
#include "alloc.h"
#include <cstddef>
#include <cstdlib>

/// @brief list节点实现
template <class T>
class list_node
{
public:
    list_node() : front(0), next(0) {}
    list_node *front;
    list_node *next;
    T val;
};

/// @brief 正向迭代器
template <class T>
class list_iterator
{
private:
    list_node<T> *ptr;

public:
    list_iterator() : ptr(0) {}
    list_iterator(list_node<T> *p) : ptr(p) {}
    list_iterator(const list_iterator &p) : ptr(p.ptr) {}
    bool operator==(const list_iterator &p) { return ptr == p.ptr; }
    bool operator!=(const list_iterator &p) { return ptr != p.ptr; }
    T &operator*() { return ptr->val; }
    T *operator->() { return &(operator*()); }
    list_iterator operator=(const list_iterator &p)
    {
        if (ptr != p.ptr)
            ptr = p.ptr;
        return *this;
    }
    list_iterator operator+(const size_t n)
    {
        list_node<T> *ret = ptr;
        for (size_t i = 0; i < n; i++)
            ret = ret->next;
        return list_iterator(ret);
    }
    list_iterator operator-(const size_t n)
    {
        list_node<T> *ret = ptr;
        for (size_t i = 0; i < n; i++)
            ret = ret->front;
        return list_iterator(ret);
    }
    list_iterator operator++()
    {
        ptr = ptr->next;
        return *this;
    }
    list_iterator operator--()
    {
        ptr = ptr->front;
        return *this;
    }
    list_iterator operator++(int)
    {
        list_node<T> *ret = ptr;
        ptr = ptr->next;
        return list_iterator(ret);
    }
    list_iterator operator--(int)
    {
        list_node<T> *ret = ptr;
        ptr = ptr->front;
        return list_iterator(ret);
    }
};

/// @brief list实现
template <class T, class Alloc = sim_alloc<list_node<T>>>
class list
{
public:
    /******************************/
    // 接口
    using node = list_node<T>;
    using type = T;
    using pointer = T *;
    using con_pointer = const T *;
    using refer = T &;
    using con_refer = const T &;
    using iterator = list_iterator<T>;
    using con_iterator = const list_iterator<T>;
    /******************************/
    // 函数
    list()
    {
        head = get_node();
        head->front = head->next = head;
    }
    list(const size_t n, const T &val)
    {
        head = get_node();
        head->front = head->next = head;
        for (size_t i = 0; i < n; ++i)
            push_back(val);
    }
    list(const std::initializer_list<T> &pval)
    {
        head = get_node();
        head->front = head->next = head;
        for (auto i = pval.begin(); i != pval.end(); ++i)
            push_back(*i);
    }
    void push_back(const T &val)
    {
        node *pnode = get_node(val);
        head->front->next = pnode;
        pnode->next = head;
        pnode->front = head->front;
        head->front = pnode;
    }
    void push_front(const T &val)
    {
        node *pnode = get_node(val);
        pnode->next = head->next;
        pnode->front = head;
        head->next->front = pnode;
        head->next = pnode;
    }
    void pop_back()
    {
        node *temp = head->front;
        head->front->front->next = head;
        head->front = head->front->front;
        put_node(temp);
    }
    void pop_front()
    {
        node *temp = head->next;
        head->next->next->front = head;
        head->next = head->next->next;
        put_node(temp);
    }
    bool empty() const { return head->front == head->next; }
    /// @brief warning: 计算list的大小，时间复杂度为O(n),不推荐使用
    size_t size() const
    {
        size_t ret = 0;
        for (auto p = head; p != head->front; p = p->next)
            ++ret;
        return ret;
    }
    iterator begin() const { return iterator(head->next); }
    iterator end() const { return iterator(head); }
    con_iterator cbegin() const { return con_iterator(head->next); }
    con_iterator cend() const { return con_iterator(head); }
    void clear()
    {
        for (auto i = head->next; i != head;)
        {
            i = i->next;
            pop_front();
        }
    }
    ~list()
    {
        clear();
        put_node(head);
    }
    /******************************/
private:
    //实际存储的数据
    node *head;
    /******************************/
    node *get_node()
    {
        node *ret = Alloc::allocate();
        ret->front = ret->next = 0;
        return ret;
    }
    node *get_node(const T &value)
    {
        node *ret = Alloc::allocate();
        ret->front = ret->next = 0;
        _construct(&(ret->val), value);
        return ret;
    }
    void put_node(node *p)
    {
        _destroy(p);
        Alloc::deallocate(p);
    }
};

#endif