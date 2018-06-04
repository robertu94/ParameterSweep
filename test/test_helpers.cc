#include <iterator_checks.hpp>

#include <Distributions.hpp>
#include <Helpers.hpp>

#include <tuple>
#include <random>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <type_traits>
#include <gtest/gtest.h>

using namespace ParameterSweep;

class HelperTest: public ::testing::Test
{
	public:
	virtual void SetUp() {

	}
};

TEST_F(HelperTest, IteratorPrimitives)
{
	//perform all the static assertions, avoid unused declaration warning
	TestForwardReadOnly<NormalFactor<int>,
		RangeFactor<int>,
		RangeFactor<int,Geometric>,
		RandomFactor<int,Distribution<std::mt19937, std::uniform_int_distribution<>>>,
		TransformFactor<std::vector<int>::iterator, std::function<int(int)>>,
		TransformFactor<std::vector<int>::iterator, std::function<float(int)>>
		> tests;
	(void)tests;
}

TEST_F(HelperTest, Values)
{
	auto normal = NormalFactor(0,1,5);
	auto range  = RangeFactor(-2,4,7);
	auto geo_range  = RangeFactor<int,Geometric>(2,16,4);
	auto trans  = TransformFactor(std::begin(normal), std::end(normal), [](int i){return i+3;});

	std::seed_seq seed;
	std::mt19937 eng(seed);
	std::uniform_int_distribution<> gen(-100,100);
	auto rand = [&gen,&eng](){return gen(eng);};

	auto random = RandomFactor(rand, 100);

	{
	std::vector<int> results_normal(std::begin(normal), std::end(normal));
	std::vector<int> expected_normal = { -5,-4,-3,-2,-1,0,1,2,3,4,5};
	EXPECT_EQ(expected_normal, results_normal);
	}
	
	{
	std::vector<int> results_range(std::begin(range), std::end(range));
	std::vector<int> expected_range = { -2,-1,0,1,2,3,4};
	EXPECT_EQ(expected_range, results_range);
	}

	{
	std::vector<int> results_range(std::begin(geo_range), std::end(geo_range));
	std::vector<int> expected_range = {2,4,8,16};
	EXPECT_EQ(expected_range, results_range);
	}
	

	{
	std::vector<int> results_transform(std::begin(trans), std::end(trans));
	std::vector<int> expected_transform = { -2,-1,0,1,2,3,4,5,6,7,8};
	EXPECT_EQ(expected_transform, results_transform);
	}

	{
	std::seed_seq seed_expected;
	std::mt19937 eng_expected(seed_expected);
	std::uniform_int_distribution<> gen_expected(-100,100);
	auto rand_expected = [&gen_expected,&eng_expected](){return gen_expected(eng_expected);};
	std::vector<int> expected_random(100);
	std::generate(std::begin(expected_random), std::end(expected_random), rand_expected);
	std::vector<int> results_random(std::begin(random), std::end(random));
	EXPECT_EQ(expected_random, results_random);
	}
}


