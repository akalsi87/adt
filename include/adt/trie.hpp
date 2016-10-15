/**
 * \file trie.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_TRIE_HPP_INCL_GUARD
#define ADT_TRIE_HPP_INCL_GUARD

#include <cassert>
#include <iostream>
#include <functional>
#include <utility>

#include <vector>
#include <memory>

namespace adt
{
    class trie;

    std::ostream& operator<<(std::ostream& os, const trie& t);

    namespace detail
    {

        template <class T>
        struct trie_node
        {
            trie_node(T&& c) : val_(c), end_word_(false), next_()
            { } 

            trie_node* find_val(const T& c)
            {
                trie_node* out = nullptr;
                for (size_t i = 0; i < next_.size(); ++i) {
                    auto node = next_[i].get();
                    if (node->val_ == c) {
                        out = node;
                        break;
                    }
                }
                return out;
            }

            trie_node* insert_val(T&& c)
            {
                assert(find_val(c) == nullptr);
                next_.emplace_back(new trie_node(std::forward<T>(c)));
                return next_.back().get();
            }

            bool has_word_end() const
            {
                return end_word_;
            }

            void end_word()
            {
                end_word_ = true;
            }
          private:
            template <class U>
            using ptr = std::unique_ptr<U>;

            T val_;
            bool end_word_;
            std::vector<ptr<trie_node>> next_;

            trie_node(const trie_node&) = delete;
            trie_node& operator=(const trie_node&) = delete;
        };

    }// namespace detail

    class trie
    {
      public:
        trie() : root_(new node('\0'))
        { }

        void insert(const std::string& s)
        {
            insert(s.c_str());
        }

        void insert(const char* s)
        {
            inserter(*this)(s);
        }

        bool contains(const std::string& s) const
        {
            return contains(s.c_str());
        }

        bool contains(const char* s) const
        {
            return searcher<false>(*this)(s) != nullptr;
        }

        bool contains_prefix(const std::string& s) const
        {
            return contains_prefix(s.c_str());
        }

        bool contains_prefix(const char* s) const
        {
            return searcher<true>(*this)(s) != nullptr;
        }
      protected:
        using node = detail::trie_node<char>;

        std::unique_ptr<node> root_;

        struct inserter
        {
            inserter(trie& t) : t_(t)
            { }

            void operator()(const char* s)
            {
                char c;
                node* n = t_.root_.get();
                while ((c = *s)) {
                    auto next = n->find_val(c);
                    if (!next) {
                        next = n->insert_val(std::move(c));
                    }
                    ++s;
                    n = next;
                }
                n->end_word();
            }
          private:
            trie& t_;
        };

        template <bool allow_partial>
        struct searcher
        {
            searcher(const trie& t) : t_(t)
            { }

            node* operator()(const char* s)
            {
                char c;
                node* n = t_.root_.get();
                while ((c = *s)) {
                    auto next = n->find_val(c);
                    if (!next) {
                        n = nullptr;
                        break;
                    }
                    ++s;
                    n = next;
                }
                return n ? ((allow_partial || n->has_word_end()) ? n : nullptr) : nullptr;
            }
          private:
            const trie& t_;
        };
    };

}// namespace adt

#endif/*ADT_TRIE_HPP_INCL_GUARD*/
