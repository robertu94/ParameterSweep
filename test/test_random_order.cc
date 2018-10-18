#include <gtest/gtest.h>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>

#include "RandomOrder.hpp"

class RandomOrderTest: public ::testing::Test
{
	protected:
	virtual void SetUp() {
		container = std::vector<int>(100);
		std::iota(container.begin(), container.end(), 0);

	  expected = std::set<int>(container.begin(), container.end());
	}

	std::vector<int> container;
	std::set<int> expected;

};

TEST_F(RandomOrderTest, GetsAllTheValues) {
	std::set<int> results;
	std::vector<int> order;

	RandomOrder<decltype(container), 223, 331> random(container);
	std::copy(std::begin(random), std::end(random), std::inserter(results, std::end(results)));
	std::copy(std::begin(random), std::end(random), std::back_inserter(order));

	EXPECT_EQ(expected, results);
	EXPECT_NE(order, container);
}

TEST_F(RandomOrderTest, IdsAreAccending) {
	RandomOrder<decltype(container), 223, 331> random(container);

	auto it = std::begin(random);
	for (size_t i = 0; i < std::size(random); ++i, ++it) {
		EXPECT_EQ(i, it.get_id());
	}
}
