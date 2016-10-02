/**
 * \file bst.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_BST_HPP_INCL_GUARD
#define ADT_BST_HPP_INCL_GUARD

#include <functional>

namespace adt
{

    namespace detail
    {

        template <class T>
        struct bst_node
        {
            bst_node<T>* left;
            bst_node<T>* right;
            T val;
        };

        template <class T>
        class bst_iterator
        {
          public:
            bst_iterator(bst_node<T>* root);

            ~bst_iterator();

            bst_iterator(const bst_iterator& rhs);

            bst_iterator& operator++();

            bst_iterator& operator=(const bst_iterator& rhs);

          private:
            bst_node<T>* root_;
            bst_node<T>* parent_;
            bst_node<T>* curr_;
        };

    } // namespace detail

    template <class T, class Comp = std::less<T>>
    class bst
    {
      public:
        using iterator = T*;

        bst();

        ~bst();

        iterator find(const T& val) const;

        std::pair<iterator, bool> insert(const T& val);

        std::pair<iterator, bool> insert(T&& val);

        size_t erase(const T& val);

        iterator erase(iterator it);
      private:
        bst_node<T>* root_;
    };

} // namespace adt

#endif/*ADT_BST_HPP_INCL_GUARD*/
