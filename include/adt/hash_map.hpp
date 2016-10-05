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

namespace adt
{

    template <class K, class V, class Hash, class Eq>
    class hash_map;

    template <class K, class V, class Hash, class Eq>
    std::ostream& operator<<(std::ostream& os, const hash_map<K, V, Hash, Eq>& m);

    namespace detail
    {

        template <class Entry, class EntryTraits>
        class hash_table
        {
          public:
            hash_table();
            ~hash_table();

            Entry* find(const Entry& e) const;

            Entry* insert(const Entry& e);
          private:
            EntryTraits traits_;
            

            hash_table(const hash_table&) = delete;
            hash_table& operator=(const hash_table&) = delete;
        };

    }// namespace detail

    template <class K, class V, class Hash, class Eq>
    class hash_map
    {
      public:
        using entry_type = std::pair<const K, V>;

        hash_map()
          : table_()
        { }

        ~hash_map() = default;

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
    };

}// namespace adt

#endif/*ADT_HASH_MAP_HPP_INCL_GUARD*/
