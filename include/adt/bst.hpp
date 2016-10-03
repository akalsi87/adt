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

    template <class T>
    class bst;

    template<typename T>
    std::ostream& operator<<(std::ostream&, const bst<T>&);

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

            bst_iterator& operator++()
            {
                node* curr = curr_;
                if (node* right = curr->right) {
                    curr_ = minimum(right);
                    return *this;
                }
                if (node* parent = curr->parent) {
                    curr_ = rightmost_parent(curr, parent);
                } else {
                    curr_ = minimum(curr->right);
                }
                return *this;
            }

            bst_iterator operator++(int)
            {
                bst_iterator it = *this;
                ++(*this);
                return it;
            }

//            bst_iterator& operator--()
//            {
//                return *this;
//            }
//
//            bst_iterator operator--(int)
//            {
//                bst_iterator it = *this;
//                --(*this);
//                return it;
//            }

            T& operator*() { return curr_->val; }

            const T& operator*() const { return curr_->val; }

            bst_iterator& operator=(const bst_iterator& rhs) = default;

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

            static node* minimum(node* n)
            {
                if (!n) { return 0; }
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

            friend class bst<T>;
        };

    } // namespace detail

    template <class T>
    class bst
    {
      public:
        using iterator = detail::bst_iterator<T>;

        bst() : root_(nullptr), leftmost_(nullptr) { }

        ~bst() { clear(); }

        iterator find(const T& val) const
        {
            node* curr = root_;
            while (curr != nullptr) {
                if (curr->val == val) {
                    // found it!
                    break;
                }
                if (curr->val > val) {
                    // search left
                    curr = curr->left;
                } else {
                    // search right
                    curr = curr->right;
                }
            }
            return curr ? iterator(curr) : end();
        }

        std::pair<iterator, bool> insert(const T& val)
        {
            return insert(T(val));
        }

        std::pair<iterator, bool> insert(T&& val)
        {
            node* prev = nullptr;
            node* curr = root_;
            bool was_smaller = true;
            bool was_always_smaller = true;
            while (curr != nullptr) {
                if (curr->val == val) {
                    // found
                    break;
                }
                prev = curr;
                if (curr->val > val) {
                    // search left
                    curr = curr->left;
                    was_smaller = true;
                } else {
                    // search right
                    curr = curr->right;
                    was_smaller = false;
                    was_always_smaller = false;
                }
            }
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
            leftmost_ = (curr == leftmost_) ? curr->parent : leftmost_;
            if (curr->left == nullptr && curr->right == nullptr) {
                /* no children case */
                iterator next = ++it;
                if (node* parent = curr->parent) {
                    if (curr == parent->left) {
                        parent->left = nullptr;
                    } else {
                        parent->right = nullptr;
                    }
                } else {
                    root_ = nullptr;
                }
                dispose(curr);
                return next;
            } else {
                return end();
            }
        }

        iterator begin()
        {
            return iterator(leftmost_);
        }

        iterator end() const
        {
            return iterator(nullptr);
        }

        void clear()
        {
            destroy_tree(root_);
            root_ = leftmost_ = nullptr;
        }
      private:
        using node = detail::bst_node<T>;
        node* root_;
        node* leftmost_;

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

        static void destroy_tree(node* n)
        {
            if (!n) { return; }
            node* lft = n->left;
            node* rgt = n->right;
            dispose(n);
            destroy_tree(lft);
            destroy_tree(rgt);
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

    template <class T>
    std::ostream& operator<<(std::ostream& os, const bst<T>& t)
    {
        os << "{";
        t.print_tree(t.leftmost_, os);
        os << "}";
        return os;
    }

} // namespace adt

#endif/*ADT_BST_HPP_INCL_GUARD*/
