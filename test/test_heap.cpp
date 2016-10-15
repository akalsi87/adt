/**
 * \file test_heap.cpp
 * \date Oct 14, 2016
 */

#include "unittest.hpp"

#include "adt/heap.hpp"
#include <algorithm>

CPP_TEST( intHeap )
{
    adt::heap<int> h;
    {
        TEST_TRUE(h.size() == 0);
    }
    {
        h.insert(5);
        h.insert(2);
        h.insert(8);
        std::cout << h << std::endl;
        TEST_TRUE(h.top() == 2);
        TEST_TRUE(h.pop() == 2);
        TEST_TRUE(h.top() == 5);
        TEST_TRUE(h.pop() == 5);
        TEST_TRUE(h.top() == 8);
        TEST_TRUE(h.pop() == 8);
        TEST_TRUE(h.size() == 0);
    }
}

std::vector<int> get_shuffled_values(int N)
{
    std::vector<int> values;
    for (int i = 0; i < N; ++i) {
        values.push_back(i);
    }
    std::random_shuffle(values.begin(), values.end());
    return values;
}

CPP_TEST( heapSort )
{
    const int N = 100000;
    auto vals = get_shuffled_values(N);

    auto copy = vals;
    std::sort(copy.begin(), copy.end());

    std::vector<int> heapvals;
    heapvals.reserve(vals.size());

    adt::heap<int> h;
    for (int v : vals) {
        h.insert(v);
    }

    for (size_t i = 0; i < copy.size(); ++i) {
        heapvals.push_back(h.pop());
    }

    TEST_TRUE(std::equal(copy.begin(), copy.end(), heapvals.begin()));
}

namespace
{

const int N = 100000;
auto vals = get_shuffled_values(N);

std::vector<int> copy = std::vector<int>(N, 0);
std::vector<int> heapvals = std::vector<int>(N, 0);

CPP_TEST( perfVecSort )
{
    std::copy(vals.begin(), vals.end(), copy.begin());
    std::sort(copy.begin(), copy.end());
}

CPP_TEST( perfHeapSort )
{
    adt::heap<int> h;
    for (int v : vals) {
        h.insert(v);
    }
    for (size_t i = 0; i < heapvals.size(); ++i) {
        heapvals[i] = h.pop();
    }
}

} // namespace