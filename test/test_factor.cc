#include <iterator_checks.hpp>
#include <Factor.hpp>
#include <vector>
#include <iterator>
#include <set>

#include <gtest/gtest.h>

using namespace ParameterSweep;

class FactorTest: public ::testing::Test
{
	public:
	virtual void SetUp() {

		example = Factor(vi, si);
	}

	std::vector<int> vi = {1,2,3,4};
	std::set<int> si = {1,2,5};
	using example_type = Factor<std::vector<int>, std::set<int>>;
	using iterator = example_type::iterator;
	example_type example = Factor(vi,si);
};

TEST_F(FactorTest, IteratorPrimitives)
{
	TestForwardWritable<int, example_type::iterator> tests;
	(void)tests;

	//is copy assignable
	{
	iterator a, b;
	a = b;
	}


	//is swappable
	{
	iterator a;
	iterator b;
	std::swap(a,b);
	}

	//is de-referenceable
	{
	auto i = *std::begin(example);
	(void)i;
	}

	//is incrementable
	{
	auto i = ++std::begin(example);
	(void)i;
	}

	//is addable
	{
		std::ptrdiff_t expected_n = 2;
		auto begin = std::begin(example);
		EXPECT_EQ(0, begin-begin);

		auto expected_second = std::begin(example)++;
		auto second = begin+1;
		EXPECT_EQ(expected_second, second);
		EXPECT_EQ(1, second-begin);


		auto expected_third = (std::begin(example)++)++;
		auto third = begin+expected_n;
		EXPECT_EQ(expected_third, third);

		auto n = third-begin;
		EXPECT_EQ(expected_n, n);

		auto expected_first = std::begin(example);
		auto first = expected_third-expected_n;
		EXPECT_EQ(expected_first, first);

		auto end = std::end(example);
		auto len = end - begin;
		EXPECT_EQ(7, len);
		EXPECT_EQ(0, end-end);
	}
}

TEST_F(FactorTest, SizeAndIteration)
{
	auto get_sizes = [](auto& factor)
	{
		size_t count = std::count_if(std::begin(factor), std::end(factor), [](auto const&){return true;});
		std::vector<int> values(std::begin(factor), std::end(factor));
		return std::make_tuple(count, values);
	};

	{
		auto [count, values] = get_sizes(example);
		std::vector<int> expected = {1,2,3,4,1,2,5};
		EXPECT_EQ(7, example.size());
		EXPECT_EQ(7, std::size(example));
		EXPECT_EQ(7, count);
		EXPECT_EQ(expected, values);
	}
}

TEST_F(FactorTest, ToParameters)
{
	std::vector<std::vector<size_t>> expecteds = {
		{0, 0, 0},
		{0, 1, 0},
		{0, 2, 0},
		{0, 3, 0},
		{1, 0, 0},
		{1, 0, 1},
		{1, 0, 2},
	};
	auto it = std::begin(example);
	for (size_t index = 0; index < example.size(); ++index, ++it) {
		auto expected = expecteds.at(index);
		{
			auto results = it.get_parameters();
			EXPECT_EQ(expected, results);
		}
		{
			auto results = example.get_parameters(index);
			EXPECT_EQ(expected, results);
		}
	}
}
