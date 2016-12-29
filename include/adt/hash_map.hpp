/**
 * \file hash_map.hpp
 * \date Oct 02, 2016
 */

#ifndef ADT_HASH_MAP_HPP_INCL_GUARD
#define ADT_HASH_MAP_HPP_INCL_GUARD

#include "adt/dlist.hpp"

#include <cassert>
#include <iostream>
#include <functional>
#include <numeric>
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
            using list_type = adt::dlist<Entry>;
            using entry_iter = typename list_type::iterator;
            using buckets_type = std::vector<entry_iter>;

            hash_table(size_t cap, float load_factor)
              : list_()
              , buckets_(cap, list_.end())
              , num_entries_(cap)
              , inc_cap_(0)
              , dec_cap_(0)
              , min_cap_(37)
              , traits_()
              , load_factor_(load_factor)
            {
                assert(load_factor > 0.0f);
                assert(load_factor < 1.0f);
                update_cap(cap);
            }

            ~hash_table()
            {
                clear();
            }

            void reserve(size_t n)
            {
                if (n > capacity()) {
                    rehash(n);
                }
            }

            void clear()
            {
                if (size() != 0) {
                    empty_contents();
                }
            }

            inline entry_iter begin() const
            {
                return list_.begin();
            }

            inline entry_iter end() const
            {
                return list_.end();
            }

            inline entry_iter find(const Entry& e) const
            {
                return find_entry_in_bucket(e, get_bucket(e));
            }

            inline entry_iter insert(const Entry& e, bool& inserted)
            {
                size_t const idx = get_bucket(e);
                auto it = find_entry_in_bucket(e, idx);
                if (it != end()) {
                    inserted = false;
                    return it;
                }
                inserted = true;
                it = list_.insert(buckets_[idx], e);
                buckets_[idx] = it;
                ++num_entries_[idx];
                if (list_.size() > inc_cap_) {
                    reserve(next_capacity(capacity()));
                }
                return it;
            }

            inline size_t erase(const Entry& e)
            {
                //assert(std::accumulate(num_entries_.begin(), num_entries_.end(), int32_t(0)) == static_cast<int32_t>(size()));
                size_t const bucket = get_bucket(e);
                entry_iter it = find_entry_in_bucket(e, bucket);
                if (it != end()) {
                    if (--num_entries_[bucket] == 0) {
                        buckets_[bucket] = end();
                    }
                    if (buckets_[bucket] == it) {
                        ++buckets_[bucket];
                    }
                    list_.erase(it);
                    //assert(num_entries_[bucket] >= 0);
                    // shrink if table is too sparse
                    if (size() < dec_cap_) {
                        rehash(next_capacity(size() + 1));
                    }
                    return 1;
                } else {
                    return 0;
                }
            }

            inline size_t capacity() const
            {
                return buckets_.size();
            }

            inline size_t size() const
            {
                return list_.size();
            }
          private:
            mutable list_type list_;
            mutable buckets_type buckets_;
            std::vector<int32_t> num_entries_;
            size_t inc_cap_;
            size_t dec_cap_;
            size_t const min_cap_;
            mutable EntryTraits traits_;
            float load_factor_;


            hash_table(const hash_table&) = delete;
            hash_table& operator=(const hash_table&) = delete;

            static size_t next_capacity(size_t c)
            {
                c = (c + (c >> 1) + (c >> 2));
                return c + (1 - (c & 1));
            }

            void empty_contents()
            {
                list_.clear();
                std::fill(buckets_.begin(), buckets_.end(), list_.end());
                std::fill(num_entries_.begin(), num_entries_.end(), int32_t(0));
            }

            void print(std::ostream& os) const
            {
                auto num_buckets = buckets_.size();
                for (size_t i = 0; i < num_buckets; ++i) {
                    os << "  [" << i << "]: ";
                    auto it = buckets_[i];
                    auto num_entries = num_entries_[i];
                    while (num_entries-- > 0) {
                        os << *it++ << " ";
                    }
                    os << "\n";
                }
            }

            inline size_t hash_entry(const Entry& e) const
            {
                return traits_.hash(e);
            }

            inline void insert_spliced(entry_iter to_move, list_type& list)
            {
                assert(list.end() != to_move);
                auto const& entry = *to_move;
                size_t const idx = get_bucket(entry);
                auto it = buckets_[idx];
                list_.splice(it, list, to_move);
                buckets_[idx] = --it;
                ++num_entries_[idx];
            }

            void update_cap(size_t new_cap)
            {
                float load_lim = load_factor_ * new_cap;
                inc_cap_ = static_cast<size_t>(load_lim);
                dec_cap_ = static_cast<size_t>(load_lim/5);
            }

            void rehash(size_t new_cap)
            {
                if (new_cap < min_cap_) { new_cap = min_cap_; }
                if (new_cap == buckets_.size()) { return; }
                if (size() == 0) { return; }
                hash_table<Entry, EntryTraits> exchange(new_cap, load_factor_);
                entry_iter it = list_.begin();
                entry_iter const end_iter = list_.end();
                while (it != end_iter) {
                    auto next = it;
                    ++next;
                    exchange.insert_spliced(it, list_);
                    it = next;
                }
                std::swap(buckets_, exchange.buckets_);
                std::swap(list_, exchange.list_);
                std::swap(num_entries_, exchange.num_entries_);
                std::swap(inc_cap_, exchange.inc_cap_);
                std::swap(dec_cap_, exchange.dec_cap_);
            }

            inline size_t get_bucket(Entry const& e) const
            {
                size_t hash = hash_entry(e);
                const size_t num_buckets = buckets_.size();
                return hash % num_buckets;
            }

            entry_iter find_entry_in_bucket(Entry const& e, size_t idx) const
            {
                //assert(std::accumulate(num_entries_.begin(), num_entries_.end(), int32_t(0)) == static_cast<int32_t>(size()));
                entry_iter it = buckets_[idx];
                auto num = num_entries_[idx];
                //assert(std::distance(it, end()) >= num);
                while (num-- > 0) {
                    if (traits_.is_equal(e, *it)) {
                        return it;
                    }
                    ++it;
                }
                return end();
            }

            template <class K, class V, class Hash, class Eq>
            friend class adt::hash_map;
        };

    }// namespace detail

    template <class K, class V, class Hash = std::hash<K>, class Eq = std::equal_to<K>>
    class hash_map
    {
        using Entry = std::pair<K const, V>;
        class EntryTraits
        {
          public:
            EntryTraits() : hash_(), eq_()
            { }

            inline size_t hash(const Entry& e)
            {
                return hash_(e.first);
            }

            inline bool is_equal(const Entry& a, const Entry& b)
            {
                return eq_(a.first, b.first);
            }
          private:
            Hash hash_;
            Eq eq_;
        };
        using table_type = detail::hash_table<Entry, EntryTraits>;
      public:
        using iterator = typename table_type::entry_iter;

        hash_map(size_t cap = 37, float load_factor = 0.75f)
          : table_(cap, load_factor)
        { }

        ~hash_map() = default;

        void clear()
        {
            table_.clear();
        }

        inline iterator begin()
        {
            return table_.begin();
        }

        inline iterator end()
        {
            return table_.end();
        }

        inline iterator find(const K& k) const
        {
            fake_key_value entry = { k, };
            return table_.find(*reinterpret_cast<Entry*>(&entry));
        }

        inline std::pair<iterator, bool> insert(const K& k, const V& v)
        {
            bool inserted = false;
            iterator res = table_.insert({k, v}, inserted);
            return std::make_pair(res, inserted);
        }

        inline size_t erase(const K& k)
        {
            fake_key_value entry = { k, };
            return table_.erase(*reinterpret_cast<Entry*>(&entry));
        }

        inline size_t capacity() const
        {
            return table_.capacity();
        }

        inline size_t size() const
        {
            return table_.size();
        }

      private:
        struct fake_key_value
        {
            K key;
            char unused_[sizeof(V)];
        };

        table_type table_;

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
