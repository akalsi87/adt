/**
 * \file hash_map.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_HASH_MAP_HPP_INCL_GUARD
#define ADT_HASH_MAP_HPP_INCL_GUARD

#include <cassert>
#include <iostream>
#include <functional>
#include <utility>
#include <vector>

namespace adt
{

    template <class K, class V, class Hash, class Eq>
    class hash_map;

    template <class K, class V, class Hash, class Eq>
    std::ostream& operator<<(std::ostream& os, const hash_map<K, V, Hash, Eq>& m);

    namespace detail
    {

        template <class A, class B>
        std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p)
        {
            os << "(" << p.first << "," << p.second << ")";
            return os;
        }

        template <class Entry, class EntryTraits>
        class hash_table
        {
          public:
            hash_table(size_t cap, float load_factor)
              : traits_()
              , buckets_(cap)
              , load_factor_(load_factor)
              , size_(0)
            {
            }

            ~hash_table()
            {
                clear();
            }

            void clear()
            {
                empty_contents(buckets_);
                size_ = 0;
            }

            Entry* find(const Entry& e) const
            {
                auto res = find_prev_and_curr(e);
                entry_node* node = res.second;
                return node ? &(node->value) : nullptr;
            }

            Entry* insert(const Entry& e, bool& inserted)
            {
                auto res = find_prev_and_curr(e);
                entry_node* node = res.second;
                if (!(inserted = !node)) {
                    return &(node->value);
                } else {
                    entry_node* new_node = create(std::move(Entry(e)));
                    entry_node** pprev = res.first;
                    assert(pprev);
                    *(pprev) = new_node;
                    ++size_;

                    auto cap = capacity();
                    if (size_ > (size_t)(load_factor_ * cap)) {
                        size_t next_cap = next_capacity(cap);
                        if (next_cap > cap) {
                            rehash(next_cap);
                        }// else { /* overflow! no rehash */ }
                    }

                    return &(new_node->value);
                }
            }

            size_t erase(const Entry& e)
            {
                auto res = find_prev_and_curr(e);
                entry_node* node = res.second;
                if (node) {
                    entry_node** pprev = res.first;
                    *(pprev) = node->next;
                    dispose(node);
                    --size_;

                    auto cap = capacity();
                    if (size_ < (size_t)(0.05f * cap)) {
                        rehash(next_capacity(size_ + 1));
                    }

                    return 1;
                } else {
                    return 0;
                }
            }

            size_t capacity() const
            {
                return buckets_.size();
            }

            size_t size() const
            {
                return size_;
            }
          private:
            struct entry_node
            {
                entry_node* next;
                Entry value;

                entry_node(Entry&& v)
                  : next(nullptr)
                  , value(std::forward<Entry>(v))
                { }
            };

            using buckets_type = std::vector<entry_node*>;

            mutable EntryTraits traits_;
            mutable buckets_type buckets_;
            float load_factor_;
            size_t size_;

            hash_table(const hash_table&) = delete;
            hash_table& operator=(const hash_table&) = delete;

            static size_t next_capacity(size_t c)
            {
                c = (c + (c >> 1) + (c >> 2));
                return c + (1 - (c & 1));
            }

            static entry_node* create(Entry&& e)
            {
                return new entry_node(std::forward<Entry>(e));
            }

            static void dispose(entry_node* n)
            {
                delete n;
            }

            static void empty_contents(buckets_type& b)
            {
                for (auto& node : b) {
                    auto node_iter = node;
                    while (node_iter) {
                        auto next = node_iter->next;
                        dispose(node_iter);
                        node_iter = next;
                    }
                    node = nullptr;
                }
            }

            void print(std::ostream& os) const
            {
                size_t idx = 0;
                for (auto node : buckets_) {
                    ++idx;
                    if (!node) { continue; }
                    os << "  [" << idx - 1 << "]: ";
                    while (node) {
                        auto next = node->next;
                        os << node->value << " ";
                        node = next;
                    }
                    os << "\n";
                }
            }

            size_t hash_entry(const Entry& e) const
            {
                return traits_.hash(e);
            }

            void rehash(size_t new_cap)
            {
                buckets_type new_buckets(new_cap);
                for (auto node : buckets_) {
                    while (node) {
                        size_t hash = hash_entry(node->value);
                        size_t idx = hash % new_cap;
                        entry_node* next = node->next;
                        node->next = nullptr;
                        entry_node* new_node = new_buckets[idx];
                        if (!new_node) {
                            new_buckets[idx] = node;
                        } else {
                            while (new_node->next) {
                                new_node = new_node->next;
                            }
                            new_node->next = node;
                        }
                        node = next;
                    }
                }
                std::swap(buckets_, new_buckets);
            }

            std::pair<entry_node**, entry_node*> find_prev_and_curr(const Entry& e) const
            {
                return find_prev_and_curr(std::move(Entry(e)));
            }

            std::pair<entry_node**, entry_node*> find_prev_and_curr(Entry&& e) const
            {
                size_t hash = hash_entry(e);
                size_t idx = hash % buckets_.size();

                entry_node** pprev = &buckets_[idx];
                entry_node* node = *pprev;
                while (node) {
                    if (traits_.is_equal(e, node->value)) {
                        break;
                    }
                    pprev = &(node->next);
                    node = node->next;
                }
                return std::make_pair(pprev, node);
            }

            template <class K, class V, class Hash, class Eq>
            friend class adt::hash_map;
        };

    }// namespace detail

    template <class K, class V, class Hash = std::hash<K>, class Eq = std::equal_to<K>>
    class hash_map
    {
      public:
        using entry_type = std::pair<const K, V>;

        hash_map(size_t cap = 37, float load_factor = 0.75f)
          : table_(cap, load_factor)
        { }

        ~hash_map() = default;

        void clear()
        {
            table_.clear();
        }

        entry_type* find(const K& k) const
        {
            return table_.find({ k, V() });
        }

        std::pair<entry_type*, bool> insert(const K& k, const V& v)
        {
            bool inserted = false;
            entry_type* res = table_.insert({k, v}, inserted);
            return std::make_pair(res, inserted);
        }

        size_t erase(const K& k)
        {
            return table_.erase({ k, V() });
        }

        size_t capacity() const
        {
            return table_.capacity();
        }

        size_t size() const
        {
            return table_.size();
        }

      private:
        class entry_traits
        {
          public:
            entry_traits() : hash_(), eq_()
            { }

            size_t hash(const entry_type& e)
            {
                return hash_(e.first);
            }

            bool is_equal(const entry_type& a, const entry_type& b)
            {
                return eq_(a.first, b.first);
            }
          private:
            Hash hash_;
            Eq eq_;
        };

        detail::hash_table<entry_type, entry_traits> table_;

        hash_map(const hash_map&) = delete;
        hash_map& operator=(const hash_map&) = delete;

        friend std::ostream& operator<< <> (std::ostream& os, const hash_map& m);

        void print_map(std::ostream& os = std::cerr) const
        {
            table_.print(os);
        }
    };

    template <class K, class V, class Hash, class Eq>
    std::ostream& operator<<(std::ostream& os, const hash_map<K, V, Hash, Eq>& m)
    {
        os << "{" << (m.size() == 0 ? "" : "\n");
        m.print_map(os);
        os << "}";
        return os;
    }

}// namespace adt

#endif/*ADT_HASH_MAP_HPP_INCL_GUARD*/
