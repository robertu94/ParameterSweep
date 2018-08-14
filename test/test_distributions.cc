#include <Distributions.hpp>
#include <algorithm>
#include <random>

#include <gtest/gtest.h>

TEST(distributions, seeding) {
	const size_t seed = 12;
	const size_t repetitions = 30;
	const size_t vec_size = 1000;

	std::seed_seq expected_seed;
	std::minstd_rand expected_gen(expected_seed);
	std::normal_distribution<float> expected_dist(10,3);
	expected_gen.seed(seed);
	std::vector<float> expected(vec_size);
	std::generate(std::begin(expected), std::end(expected), [&expected_dist,&expected_gen]{ return expected_dist(expected_gen); });

	std::vector<float> actual(vec_size);
	auto dist_sp = make_dist_from_tuple<std::normal_distribution<float>>(std::make_tuple(10,3));
	RandomNumberGenerator<float>& dist_from_tup = *dist_sp;
	for (size_t i = 0; i < repetitions; ++i) {
		dist_from_tup.seed(seed);
		std::generate(std::begin(actual), std::end(actual), [&dist_from_tup](){return dist_from_tup();});
		EXPECT_EQ(expected, actual);
	}
}

TEST(distributions, construction) {
	std::seed_seq expected_seed;
	std::minstd_rand expected_gen(expected_seed);
	std::normal_distribution<float> expected_dist(10,3);
	
	std::vector<float> expected(100);
	std::generate(std::begin(expected), std::end(expected), [&expected_dist,&expected_gen]{ return expected_dist(expected_gen); });

	std::vector<float> actual(100);
	std::seed_seq seed;
	std::minstd_rand gen(expected_seed);
	std::normal_distribution<float> dist(10,3);
	auto func = make_dist(gen, dist);
	std::generate(std::begin(actual), std::end(actual), *func);
	EXPECT_EQ(expected, actual) << "from make_dist";

	auto dist_sp = make_dist_from_tuple<std::normal_distribution<float>>(std::make_tuple(10,3));
	RandomNumberGenerator<float>& dist_from_tup = *dist_sp;
	std::generate(std::begin(actual), std::end(actual), [&dist_from_tup](){return dist_from_tup();});
	EXPECT_EQ(expected, actual) << "from make_dist_from_tuple";
}
