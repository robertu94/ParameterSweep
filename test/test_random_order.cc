#include <gtest/gtest.h>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>

#include "RandomOrder.hpp"

TEST(RandomOrder, GetsAllTheValues) {
	std::vector<int> container(100);
	std::iota(container.begin(), container.end(), 0);

	std::set<int> expected(container.begin(), container.end());
	std::set<int> results;
	std::vector<int> order;

	RandomOrder<decltype(container), 223, 331> random(container);
	std::copy(std::begin(random), std::end(random), std::inserter(results, std::end(results)));
	std::copy(std::begin(random), std::end(random), std::back_inserter(order));

	EXPECT_EQ(expected, results);
	EXPECT_NE(order, container);


}
