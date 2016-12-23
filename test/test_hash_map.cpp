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
        size_t prev_size = map.size();
        auto rv = map.insert(val, "num");
        TEST_TRUE(rv.first->first == val);
        TEST_TRUE(std::strcmp(rv.first->second, "num") == 0);
        TEST_TRUE(map.size() == (rv.second ? ++prev_size : prev_size));
        if (i == 20) {
            std::cout << map;
        }
    }

    size_t sz = map.size();
    for (auto key : keys) {
        size_t removed = map.erase(key);
        TEST_TRUE(map.size() == (removed ? --sz : sz));
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
#if !defined(_WIN32)
        const char* env = getenv("N");
#else
        char * buffer = 0;
        size_t len = 0;
        _dupenv_s(&buffer, &len, "N");
        assert(len <= 47);
        char enva[48] = { 0, };
        const char * env = 0;
        if (buffer) {
            strcpy_s(enva, buffer);
            env = enva;
            free(buffer);
        }
#endif
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