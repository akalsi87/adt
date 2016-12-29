/**
 * \file test_dlist.cpp
 * \date Dec 26, 2016
 */

#include "unittest.hpp"

#include "adt/dlist.hpp"

CPP_TEST( dlist_of_int )
{
    using list = adt::dlist<int>;
    list l;
    {
        TEST_TRUE(l.empty());
        TEST_TRUE(l.size() == 0);
        TEST_TRUE(l.begin() == l.end());
    }
    {
        l.push_back(-1);
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 1);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --l.end());
        TEST_TRUE(*l.begin() == -1);
    }
    {
        l.push_front(-2);
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 2);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --(--l.end()));
        TEST_TRUE(++(++l.begin()) == l.end());
        TEST_TRUE(*l.begin() == -2);
        TEST_TRUE(*(++l.begin()) == -1);
    }
    {
        l.clear();
        TEST_TRUE(l.empty());
        TEST_TRUE(l.size() == 0);
    }

    {
        l.push_back(-1);
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 1);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --l.end());
        TEST_TRUE(*l.begin() == -1);
    }
    {
        l.push_front(-2);
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 2);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --(--l.end()));
        TEST_TRUE(++(++l.begin()) == l.end());
        TEST_TRUE(*l.begin() == -2);
        TEST_TRUE(*(++l.begin()) == -1);
    }
    {
        l.insert(--l.end(), 0);
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 3);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --(--(--l.end())));
        TEST_TRUE(++(++(++l.begin())) == l.end());
        TEST_TRUE(*l.begin() == -2);
        TEST_TRUE(*(++l.begin()) == 0);
        TEST_TRUE(*(++(++l.begin())) == -1);
    }
    {
        l.splice(l.begin(), l, --l.end());
        TEST_TRUE(!l.empty());
        TEST_TRUE(l.size() == 3);
        TEST_TRUE(l.begin() != l.end());
        TEST_TRUE(l.begin() == --(--(--l.end())));
        TEST_TRUE(++(++(++l.begin())) == l.end());
        TEST_TRUE(*l.begin() == -1);
        TEST_TRUE(*(++l.begin()) == -2);
        TEST_TRUE(*(++(++l.begin())) == 0);
    }
}
