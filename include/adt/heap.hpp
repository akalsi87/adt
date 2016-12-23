/**
 * \file heap.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_HEAP_HPP_INCL_GUARD
#define ADT_HEAP_HPP_INCL_GUARD

#include <cassert>
#include <iostream>
#include <functional>
#include <utility>
#include <vector>

namespace adt
{

    template <class T, class Comparator>
    class heap;

    template <class U, class C>
    std::ostream& operator<<(std::ostream&, const heap<U, C>&);

    template <class T, class Comparator = std::greater<T>>
    class heap
    {
      public:
        heap() : arr_(), cmp_()
        { }

        size_t size() const
        {
            return arr_.size();
        }

        const T& top() const
        {
            assert(size() != 0);
            // assert(valid_heap());
            return arr_.front();
        }

        T pop()
        {
            assert(size() != 0);
            // assert(valid_heap());
            std::swap(arr_.front(), arr_.back());
            T val = arr_.back();
            arr_.pop_back();
            swim_down(0);
            // assert(valid_heap());
            return val;
        }

        void insert(const T& v)
        {
            insert(std::move(T(v)));
        }

        void insert(T&& v)
        {
            // assert(valid_heap());
            arr_.emplace_back(v);
            swim_up(arr_.size() - 1);
            // assert(valid_heap());
        }
      protected:
        std::vector<T> arr_;
        Comparator cmp_;

        heap(const heap&) = delete;
        heap& operator=(const heap&) = delete;

        static int left_child(size_t i)
        {
            return ((i+1) << 1) - 1;
        }

        static int right_child(size_t i)
        {
            return left_child(i) + 1;
        }

        static int parent(size_t i)
        {
            return i == 0 ? 0 : ((i+1) >> 1) - 1;
        }

        void swim_up(size_t idx)
        {
            if (size() < 2) { return; }
            size_t p = parent(idx);
            while (compare(idx, p)) {
                swap(idx, p);
                idx = p;
                if (idx == 0) {
                    break;
                } else {
                    p = parent(p);
                }
            }
        }

        void swim_down(size_t idx)
        {
            while (idx < arr_.size()) {
                size_t l = left_child(idx);
                size_t r = right_child(idx);
                bool has_l = (l < arr_.size());
                bool has_r = (r < arr_.size());
                size_t next;
                if (has_l && has_r) {
                    bool small_l = compare(idx, l);
                    bool small_r = compare(idx, r);
                    if (small_l && small_r) {
                        // done!
                        break;
                    } else if (small_l) {
                        next = r;
                    } else if (small_r) {
                        next = l;
                    } else {
                        next = compare(l, r) ? l : r;
                    }
                } else if (has_l && !compare(idx, l)) {
                    next = l;
                } else {
                    // done!
                    break;
                }
                swap(idx, next);
                idx = next;
            }
        }

        bool valid_heap(size_t idx = 0) const
        {
            size_t l = left_child(idx);
            size_t r = right_child(idx);
            bool has_l = (l < arr_.size());
            bool has_r = (r < arr_.size());
            if (has_l && has_r) {
                return compare(idx, l) && compare(idx, r) && valid_heap(l) && valid_heap(r);
            } else if (has_l) {
                return compare(idx, l) && valid_heap(l);
            } else {
                return true;
            }
        }

        bool compare(size_t a, size_t b) const
        {
            return !cmp_(arr_[a], arr_[b]);
        }

        void swap(size_t a, size_t b)
        {
            std::swap(arr_[a], arr_[b]);
        }

        void print(std::ostream& os = std::cerr) const
        {
            os << "[ ";
            for (const T& v : arr_) {
                os << v << ' ';
            }
            os << "]";
        }

        friend std::ostream& operator<< <> (std::ostream& os, const heap& t);
    };

    template <class U, class C>
    std::ostream& operator<<(std::ostream& os, const heap<U, C>& t)
    {
        t.print(os);
        return os;
    }

} // namespace adt

#endif/*ADT_HEAP_HPP_INCL_GUARD*/
