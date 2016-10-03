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
    {// insert 10 elements
        std::vector<int> values;
        for (int i = 0; i < 10; ++i) {
            values.push_back(i);
        }
        std::random_shuffle(values.begin(), values.end());

        for (int i : values) {
            auto r = intSet.insert(i);
            TEST_TRUE(*(r.first) == i);
            TEST_TRUE(r.second);
            std::cout << intSet << std::endl;
        }

        int count = 0;
        for (int i : intSet) {
            TEST_TRUE(i == count++);
        }
        intSet.clear();
        TEST_TRUE(intSet.begin() == intSet.end());
    }
}
