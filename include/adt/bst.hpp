/**
 * \file bst.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_BST_HPP_INCL_GUARD
#define ADT_BST_HPP_INCL_GUARD

#include <cassert>
#include <iostream>
#include <functional>
#include <utility>

namespace adt
{

    template <class T, class Comparator>
    class bst;

    template <class U, class C>
    std::ostream& operator<<(std::ostream&, const bst<U, C>&);

    namespace detail
    {

        template <class T>
        class bst_node
        {
          public:
            bst_node* left;
            bst_node* right;
            bst_node* parent;
            T val;

            bst_node(T&& v) : left(nullptr), right(nullptr), parent(nullptr), val(v) { }
            ~bst_node() { }
          private:
            bst_node(const bst_node&) = delete;
            bst_node& operator=(const bst_node&) = delete;
        };

        template <class T>
        class bst_iterator
        {
          private:
            using node = bst_node<T>;
          public:
            bst_iterator(node* curr)
              : curr_(curr)
            { }

            bst_iterator(const bst_iterator& rhs) = default;
            bst_iterator& operator=(const bst_iterator& rhs) = default;

            bst_iterator& operator++()
            {
                curr_ = find_next_node(curr_);
                return *this;
            }

            bst_iterator operator++(int)
            {
                bst_iterator it = *this;
                ++(*this);
                return it;
            }

            bst_iterator& operator--()
            {
                curr_ = find_prev_node(curr_);
                return *this;
            }

            bst_iterator operator--(int)
            {
                bst_iterator it = *this;
                --(*this);
                return it;
            }

            T& operator*() { return curr_->val; }

            const T& operator*() const { return curr_->val; }

            bool operator==(const bst_iterator& rhs) const
            {
                return curr_ == rhs.curr_;
            }

            bool operator!=(const bst_iterator& rhs) const
            {
                return !(*this == rhs);
            }
          private:
            node* curr_;

            static node* maximum(node* n)
            {
                if (!n) { return nullptr; }
                while (node* right = n->right) { n = right; }
                return n;
            }

            static node* minimum(node* n)
            {
                if (!n) { return nullptr; }
                while (node* left = n->left) { n = left; }
                return n;
            }

            static node* rightmost_parent(node* curr, node* parent)
            {
                while (curr == parent->right) {
                    curr = parent;
                    parent = parent->parent;
                    if (!parent) {
                        break;
                    }
                }
                return parent;
            }

            static node* leftmost_parent(node* curr, node* parent)
            {
                while (curr == parent->left) {
                    curr = parent;
                    parent = parent->parent;
                    if (!parent) {
                        break;
                    }
                }
                return parent;
            }

            static node* find_next_node(node* n)
            {
                if (node* right = n->right) {
                    return minimum(right);
                }
                if (node* parent = n->parent) {
                    return rightmost_parent(n, parent);
                } else {
                    // at tree root
                    return minimum(n->right);
                }
            }

            static node* find_prev_node(node* n)
            {
                if (node* left = n->left) {
                    return maximum(n->left);
                }
                if (node* parent = n->parent) {
                    return leftmost_parent(n, parent);
                } else {
                    return maximum(n->left);
                }
            }

            template <class U, class Comparator>
            friend class bst;
        };

        template <class T>
        class bst_reverse_iterator
        {
          private:
            using node = bst_node<T>;
          public:
            bst_reverse_iterator(node* curr)
              : it_(curr)
            { }

            bst_reverse_iterator(const bst_reverse_iterator& rhs) = default;
            bst_reverse_iterator& operator=(const bst_reverse_iterator& rhs) = default;

            bst_reverse_iterator& operator++()
            {
                --it_;
                return *this;
            }

            bst_reverse_iterator operator++(int)
            {
                auto it = *this;
                ++(*this);
                return it;
            }

            bst_reverse_iterator& operator--()
            {
                ++it_;
                return *this;
            }

            bst_reverse_iterator operator--(int)
            {
                auto it = *this;
                --(*this);
                return it;
            }

            T& operator*() { return *it_; }

            const T& operator*() const { return *it_; }

            bool operator==(const bst_reverse_iterator& rhs) const
            {
                return it_ == rhs.it_;
            }

            bool operator!=(const bst_reverse_iterator& rhs) const
            {
                return !(*this == rhs);
            }
          private:
            bst_iterator<T> it_;
        };

    } // namespace detail

    template <class T, class Comparator = std::less<T>>
    class bst
    {
      public:
        using iterator = detail::bst_iterator<T>;
        using reverse_iterator = detail::bst_reverse_iterator<T>;

        bst()
          : root_(nullptr)
          , leftmost_(nullptr)
          , rightmost_(nullptr)
          , size_(0)
          , cmp_()
        { }

        ~bst()
        {
            clear();
        }

        iterator find(const T& val) const
        {
            node* curr = root_;
            while (curr != nullptr) {
                switch (compare(curr->val, val)) {
                case EQ:
                    goto find_loop_end;
                case LT:
                    curr = curr->right;
                    break;
                case GT:
                    curr = curr->left;
                    break;
                }
            }
        find_loop_end:
            return curr ? iterator(curr) : end();
        }

        std::pair<iterator, bool> insert(const T& val)
        {
            return insert(std::move(T(val)));
        }

        std::pair<iterator, bool> insert(T&& val)
        {
            node* prev = nullptr;
            node* curr = root_;
            bool was_smaller = true;
            bool was_larger = true;
            bool was_always_smaller = true;
            bool was_always_larger = true;
            while (curr != nullptr) {
                switch (compare(curr->val, val)) {
                case EQ: // found!
                    goto insert_loop_end;
                case LT: // search right
                    prev = curr;
                    curr = curr->right;
                    was_smaller = false;
                    was_larger = true;
                    was_always_smaller = false;
                    break;
                case GT: // search left
                    prev = curr;
                    curr = curr->left;
                    was_smaller = true;
                    was_larger = false;
                    was_always_larger = false;
                    break;
                }
            }
        insert_loop_end:
            if (curr) {
                return std::make_pair(iterator(curr), false);
            } else {
                node* new_node = create(std::forward<T>(val));
                new_node->parent = prev;
                if (prev) {
                    if (was_smaller) {
                        prev->left = new_node;
                    } else {
                        prev->right = new_node;
                    }
                } else {
                    root_ = new_node;
                }
                if (was_smaller && was_always_smaller) {
                    leftmost_ = new_node;
                }
                if (was_larger && was_always_larger) {
                    rightmost_ = new_node;
                }
                ++size_;
                return std::make_pair(iterator(new_node), true);
            }
        }

        size_t erase(const T& val)
        {
            auto it = find(val);
            if (it != end()) {
                erase(it);
                return 1;
            } else {
                return 0;
            }
        }

        iterator erase(iterator it)
        {
            node* curr = it.curr_;
            assert(curr);
            leftmost_ = (curr == leftmost_) ? iterator::find_next_node(leftmost_) : leftmost_;
            rightmost_ = (curr == rightmost_) ? iterator::find_prev_node(rightmost_) : rightmost_;

            auto next = ++it;
            node* replacement;
            auto left = curr->left;
            auto right = curr->right;
            if (left == nullptr && right == nullptr) {
                replacement = nullptr;
            } else if (left == nullptr) {
                replacement = right;
            } else if (right == nullptr) {
                replacement = left;
            } else {
                node* new_parent_for_left = right;
                while (new_parent_for_left->left != nullptr) {
                    new_parent_for_left = new_parent_for_left->left;
                }
                new_parent_for_left->left = left;
                left->parent = new_parent_for_left;
                replacement = right;
            }

            // set parentage
            node* replacement_parent = nullptr;
            if (node* parent = curr->parent) {
                if (curr == parent->left) {
                    parent->left = replacement;
                } else {
                    parent->right = replacement;
                }
                replacement_parent = parent;
            } else {
                root_ = replacement;
            }

            if (replacement) {
                replacement->parent = replacement_parent;
            }

            // update size
            --size_;

            return next;
        }

        iterator begin()
        {
            return iterator(leftmost_);
        }

        iterator end() const
        {
            return iterator(nullptr);
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(rightmost_);
        }

        reverse_iterator rend() const
        {
            return reverse_iterator(nullptr);
        }

        size_t size() const
        {
            return size_;
        }

        void clear()
        {
            auto it = begin();
            const auto finish = end();
            for(; it != finish; it = erase(it)) { }
            assert(root_ == nullptr);
            assert(leftmost_ == nullptr);
            assert(rightmost_ == nullptr);
            assert(size_ == 0);
        }
      private:
        using node = detail::bst_node<T>;
        node* root_;
        node* leftmost_;
        node* rightmost_;
        size_t size_;
        Comparator cmp_;

        enum comp_result_t
        {
            EQ,
            LT,
            GT
        };

        comp_result_t compare(const T& a, const T& b) const
        {
            return (!cmp_(a, b))
                       ? ((!cmp_(b, a)) ? EQ : GT)
                       : LT;
        }

        // don't allow copies yet
        bst(const bst&) = delete;
        bst& operator=(const bst&) = delete;

        static node* create(T&& val)
        {
            return new node(std::forward<T>(val));
        }

        static void dispose(node* n)
        {
            delete n;
        }

        static void print_tree(node* n, std::ostream& os)
        {
            if (!n) { return; }
            iterator it(n);
            const iterator end(nullptr);
            while (it != end) {
                os << " " << *it++ << " ";
            }
        }

        friend std::ostream& operator<< <> (std::ostream& os, const bst& t);
    };

    template <class U, class C>
    std::ostream& operator<<(std::ostream& os, const bst<U, C>& t)
    {
        os << "{";
        t.print_tree(t.leftmost_, os);
        os << "}";
        return os;
    }

} // namespace adt

#endif/*ADT_BST_HPP_INCL_GUARD*/
