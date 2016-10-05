/**
 * \file test_bst.cpp
 * \date Oct 02, 2016
 */

#include <vector>
#include <algorithm>
#include "unittest.hpp"

#include "adt/bst.hpp"

CPP_TEST( bstInt )
{
    adt::bst<int> intSet;
    {// test empty
        TEST_TRUE(intSet.find(1) == intSet.end());
        std::cout << "Empty set: " << intSet << std::endl;
    }
    {// insert single element
        auto itInsPair = intSet.insert(5);
        TEST_TRUE(*(itInsPair.first) == 5);
        TEST_TRUE(itInsPair.second);
        TEST_TRUE(*intSet.begin() == 5);
        TEST_TRUE(intSet.find(5) != intSet.end());
        TEST_TRUE(*intSet.find(5) == 5);
        std::cout << "Set with 5: " << intSet << std::endl;
        TEST_TRUE(intSet.erase(5) == 1);
        std::cout << "Empty set: " << intSet << std::endl;
        itInsPair = intSet.insert(5);
        TEST_TRUE(*(itInsPair.first) == 5);
        TEST_TRUE(itInsPair.second);
        TEST_TRUE(*intSet.begin() == 5);
        TEST_TRUE(intSet.find(5) != intSet.end());
        TEST_TRUE(*intSet.find(5) == 5);
        TEST_TRUE(intSet.erase(intSet.find(5)) == intSet.end());
    }
    {// insert N elements
        int N = 32;
        std::vector<int> values;
        for (int i = 0; i < N; ++i) {
            values.push_back(i);
        }
        std::random_shuffle(values.begin(), values.end());

        for (int i : values) {
            auto r = intSet.insert(i);
            TEST_TRUE(*(r.first) == i);
            TEST_TRUE(r.second);
            // std::cout << intSet << std::endl;
        }

        auto it = ++(intSet.begin());
        auto itPrev = intSet.begin();
        for (; it != intSet.end(); ++it, ++itPrev) {
            auto prevCopy = itPrev;
            auto currCopy = it;
            TEST_TRUE((++prevCopy) == it);
            TEST_TRUE((--currCopy) == itPrev);
        }

        int count = 0;
        for (int i : intSet) {
            TEST_TRUE(i == count++);
        }

        // erase value not present
        TEST_TRUE(intSet.erase(N) == 0);

        // insert value already present
        auto itAndBool = intSet.insert(0);
        TEST_TRUE(*itAndBool.first == 0);
        TEST_TRUE(itAndBool.second == false);

        count--;
        for(auto it = intSet.rbegin(); it != intSet.rend(); ++it) {
            TEST_TRUE(*it == count--);
        }

        std::random_shuffle(values.begin(), values.end());
        // remove values
        TEST_TRUE(static_cast<int>(intSet.size()) == N);
        size_t sz = intSet.size();
        for (int i : values) {
            std::cout << "Removing entry: " << i << std::endl;
            TEST_TRUE(intSet.erase(i) == 1);
            TEST_TRUE(intSet.find(i) == intSet.end());
            TEST_TRUE(intSet.size() == --sz);
            // std::cout << "Set now: " << intSet << std::endl;
        }

        intSet.clear();
        TEST_TRUE(intSet.begin() == intSet.end());
    }
}
