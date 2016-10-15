/**
 * \file test_hash_map.cpp
 * \date Oct 05, 2016
 */

#include "adt/hash_map.hpp"
#include "unittest.hpp"

#include <cstring>

CPP_TEST( hashMapIntToCString )
{
    adt::hash_map<int, const char*> map;

    std::cout << map << std::endl;

    auto N = 1000;

    std::vector<int> keys(N);
    for (int i = 0; i < N; ++i) {
        auto val = std::rand();
        keys[i] = val;
        auto rv = map.insert(val, "num");
        TEST_TRUE(rv.second);
        TEST_TRUE(rv.first->first == val);
        TEST_TRUE(std::strcmp(rv.first->second, "num") == 0);
        TEST_TRUE(map.size() == (size_t)(i + 1));
        if (i == 20) {
            std::cout << map;
        }
    }

    TEST_TRUE(map.size() == (size_t)N);
    size_t sz = N;
    for (auto key : keys) {
        TEST_TRUE(map.erase(key) == 1);
        TEST_TRUE(map.size() == --sz);
    }

    TEST_TRUE(map.erase(1) == 0);

    std::cout << map << std::endl;
}

#include <unordered_map>

namespace performance
{

    int N = 1000000;

    std::vector<int> generate_keys()
    {
        const char* env = getenv("N");
        if (env) {
            N = std::atoi(env);
        }
        std::vector<int> keys(N);
        for (int i = 0; i < N; ++i) {
            keys[i] = std::rand();
        }
        return keys;
    }

    const std::vector<int> keys = generate_keys();

    adt::hash_map<int, const char*> adt_map;
    std::unordered_map<int, const char*> std_map;

    CPP_TEST( pStdHashMap_insert )
    {
        for (auto key : keys) {
            std_map.insert({key, "num"});
        }
    }

    CPP_TEST( pAdtHashMap_insert )
    {
        for (auto key : keys) {
            adt_map.insert(key, "num");
        }
    }

    CPP_TEST( pStdHashMap_find )
    {
        for (auto key : keys) {
            auto it = std_map.find(key);
            if (it == std_map.end()) {
                exit(1);
            }
        }
    }

    CPP_TEST( pAdtHashMap_find )
    {
        for (auto key : keys) {
            auto p = adt_map.find(key);
            if (!p) {
                exit(1);
            }
        }
    }

    CPP_TEST( pStdHashMap_clear )
    {
        std_map.clear();
    }

    CPP_TEST( pAdtHashMap_clear )
    {
        adt_map.clear();
    }

    CPP_TEST( bucketCounts )
    {
        std::cout << "std_map has " << std_map.bucket_count() << " buckets\n";
        std::cout << "adt_map has " << adt_map.capacity() << " buckets\n";
    }

}// namespace performance