/**
 * \file dlist.hpp
 * \date Dec 25, 2016
 */

#ifndef ADT_DLIST_HPP
#define ADT_DLIST_HPP

#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

namespace adt
{

template <class T>
class dlist;

namespace detail
{

template <class T>
struct dlist_node
{
    dlist_node* prev;
    dlist_node* next;
    T value;

    dlist_node(T&& val) : prev(), next(), value(std::forward<T>(val))
    { }

    dlist_node(T const& val) : prev(), next(), value(val)
    { }

    static dlist_node* create(T&& val)
    {
        return new dlist_node(std::forward<T>(val));
    }

    static dlist_node* create(T const& val)
    {
        return new dlist_node(val);
    }

    static void dispose(dlist_node* node)
    {
        delete node;
    }
};

template <class T>
dlist_node<T>* get_end(::adt::dlist<T>& l);

template <class T>
class dlist_iterator
{
    dlist_node<T>* node_;
    ::adt::dlist<T>* list_;
  public:
    dlist_iterator(dlist_iterator const& rhs) = default;
    dlist_iterator& operator=(dlist_iterator const&) = default;
    ~dlist_iterator() = default;

    T& operator*()
    {
        return node_->value;
    }

    T const& operator*() const
    {
        return node_->value;
    }

    T* operator->()
    {
        return &(node_->value);
    }

    T const* operator->() const
    {
        return &(node_->value);
    }

    dlist_iterator& operator++()
    {
        node_ = node_->next;
        return *this;
    }

    dlist_iterator operator++(int)
    {
        dlist_iterator to_ret(node_, *list_);
        node_ = node_->next;
        return to_ret;
    }

    dlist_iterator& operator--()
    {
        if (!node_) {
            node_ = get_end(*list_);
        } else {
            node_ = node_->prev;
        }
        return *this;
    }

    bool operator==(dlist_iterator const& rhs) const
    {
        return node_ == rhs.node_;
    }

    bool operator!=(dlist_iterator const& rhs) const
    {
        return !(*this == rhs);
    }
  private:
    friend class adt::dlist<T>;

    dlist_iterator(dlist_node<T>* node, ::adt::dlist<T>& l) : node_(node), list_(&l)
    {
    }
};

} // namespace detail

template <class T>
class dlist
{
  private:
    using node_type = detail::dlist_node<T>;
  public:
    using iterator = detail::dlist_iterator<T>;

    dlist() : front_(nullptr), rear_(nullptr), size_(0)
    { }
    
    ~dlist()
    {
        clear();
    }

    iterator begin()
    {
        return iterator(front_, *this);
    }

    iterator end()
    {
        return iterator(nullptr, *this);
    }
    
    size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }
    
    void push_back(T const& t)
    {
        push_back_node(node_type::create(t));
    }

    void push_back(T&& t)
    {
        push_back_node(node_type::create(std::forward<T>(t)));
    }
    
    void push_front(T const& t)
    {
        push_front_node(node_type::create(t));
    }

    void push_front(T&& t)
    {
        push_front_node(node_type::create(std::forward<T>(t)));
    }
    
    iterator insert(iterator before, T const& t)
    {
        auto next = before.node_;
        if (next == front_) {
            push_front(t);
            return iterator(front_, *this);
        } else if (next == nullptr) {
            push_back(t);
            return iterator(rear_, *this);
        } else {
            insert_before(node_type::create(t), next);
            return iterator(next->prev, *this);
        }
    }

    iterator insert(iterator before, T&& t)
    {
        auto next = before.node_;
        if (next == front_) {
            push_front(std::forward<T>(t));
            return iterator(front_, *this);
        } else if (next == nullptr) {
            push_back(std::forward<T>(t));
            return iterator(rear_, *this);
        } else {
            insert_before(node_type::create(std::forward<T>(t)), next);
            return iterator(next->prev, *this);
        }
    }

    iterator erase(iterator to_rem_iter)
    {
        auto node_to_rem = to_rem_iter.node_;
        assert(node_to_rem);
        auto prev = node_to_rem->prev;
        auto next = node_to_rem->next;
        if (prev == nullptr) {
            front_ = next;
        } else {
            prev->next = next;
        }
        if (next == nullptr){
            rear_ = prev;
        } else {
            next->prev = prev;
        }
        --size_;
        node_type::dispose(node_to_rem);
        return iterator(next, *this);
    }

    iterator splice(iterator before, dlist& to_move_from, iterator to_move)
    {
        auto before_node = before.node_;
        auto to_move_node = to_move.node_;
        to_move_from.unlink(to_move_node);
        --to_move_from.size_;
        if (before_node == front_) {
            push_front_node(to_move_node);
        } else if (before_node == nullptr) {
            push_back_node(to_move_node);
        } else {
            insert_before(to_move_node, before_node);
        }
        return iterator(to_move_node, *this);
    }

    void clear()
    {
        auto it = begin();
        const auto e = end();
        while (it != e) {
            it = erase(it);
        }
    }

    void swap(dlist& rhs)
    {
        if (this != &rhs) {
            std::swap(front_, rhs.front_);
            std::swap(rear_, rhs.rear_);
            std::swap(size_, rhs.size_);
        }
    }
  private:
    node_type* front_;
    node_type* rear_;
    size_t size_;

    void push_back_node(node_type* node)
    {
        auto old_rear = rear_;
        rear_ = node;
        if (old_rear) {
            old_rear->next = node;
        }
        node->prev = old_rear;
        node->next = nullptr;
        if (++size_ == 1) {
            front_ = node;
        }
    }

    void push_front_node(node_type* node)
    {
        auto old_front = front_;
        front_ = node;
        if (old_front) {
            old_front->prev = node;
        }
        node->prev = nullptr;
        node->next = old_front;
        if (++size_ == 1) {
            rear_ = node;
        }
    }

    void insert_before(node_type* node, node_type* next)
    {
        auto prev = next->prev;
        node->next = next;
        node->prev = prev;
        prev->next = node;
        next->prev = node;
        ++size_;
    }

    void unlink(node_type* node)
    {
        if (auto prev = node->prev) {
            prev->next = node->next;
        } else {
            front_ = node->next;
        }
        if (auto next = node->next) {
            next->prev = node->prev;
        } else {
            rear_ = node->prev;
        }
    }

    template <class U>
    friend detail::dlist_node<U>* detail::get_end(dlist<U>& l);
};

namespace detail
{

template <class T>
dlist_node<T>* get_end(dlist<T>& l)
{
    return l.rear_;
}

} // namespace detail
} // namespace adt

#endif/*ADT_DLIST_HPP*/
