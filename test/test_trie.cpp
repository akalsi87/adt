/**
 * \file test_trie.cpp
 * \date Oct 15, 2016
 */

#include "unittest.hpp"

#include "adt/trie.hpp"

const char* wordlist[] = {
    "foo",
    "bar",
    "base",
    "zack",
    "baz",
    nullptr
};

CPP_TEST( trie )
{
    adt::trie t;
    auto testWord = [&] (const char* s) -> void
                    {
                        std::string str(s);
                        TEST_TRUE(t.contains(s) == false);
                        TEST_TRUE(t.contains_prefix(s) == false);
                        t.insert(s);
                        TEST_TRUE(t.contains(s));
                        TEST_TRUE(t.contains_prefix(s));
                        for (size_t i = 0; i < str.size(); ++i) {
                            TEST_TRUE(t.contains_prefix(str.substr(0, i)));
                        }
                    };
    const char** pword = wordlist;
    while (*pword) {
        testWord(*pword);
        ++pword;
    }
}
