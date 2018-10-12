#include <iterator_checks.hpp>

#include <Distributions.hpp>
#include <Helpers.hpp>
#include <ParameterSweep.hpp>

#include <functional>
#include <gtest/gtest.h>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

using namespace ParameterSweep;

class HelperTest : public ::testing::Test
{
public:
  virtual void SetUp() {}
};

TEST_F(HelperTest, IteratorPrimitives)
{
  // perform all the static assertions, avoid unused declaration warning
  TestForwardReadOnly<
    NormalFactor<int>, RangeFactor<int>, RangeFactor<int, Geometric>,
    RandomFactor<int,
                 Distribution<std::mt19937, std::uniform_int_distribution<>>>,
    TransformFactor<std::vector<int>, std::function<int(int)>>,
    TransformFactor<std::vector<int>, std::function<float(int)>>>
    tests;
  (void)tests;
}

TEST_F(HelperTest, Values)
{
  auto normal = NormalFactor(0, 1, 5);
  auto range = RangeFactor(-2, 4, 7);
  auto geo_range = RangeFactor<int, Geometric>(2, 16, 4);
  auto trans = TransformFactor(normal, [](int i) { return i + 3; });

  std::seed_seq seed;
  std::mt19937 eng(seed);
  std::uniform_int_distribution<> gen(-100, 100);
  auto rand = [&gen, &eng]() { return gen(eng); };

  auto random = RandomFactor(rand, 100);

  {
    std::vector<int> results_normal(std::begin(normal), std::end(normal));
    std::vector<int> expected_normal = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5 };
    EXPECT_EQ(expected_normal, results_normal);
    EXPECT_EQ(std::size(expected_normal), std::size(results_normal));
  }

  {
    std::vector<int> results_range(std::begin(range), std::end(range));
    std::vector<int> expected_range = { -2, -1, 0, 1, 2, 3, 4 };
    EXPECT_EQ(expected_range, results_range);
    EXPECT_EQ(std::size(expected_range), std::size(range));
  }

  {
    std::vector<int> results_range(std::begin(geo_range), std::end(geo_range));
    std::vector<int> expected_range = { 2, 4, 8, 16 };
    EXPECT_EQ(expected_range, results_range);
    EXPECT_EQ(std::size(expected_range), std::size(geo_range));
  }

  {
    std::vector<int> results_transform(std::begin(trans), std::end(trans));
    std::vector<int> expected_transform = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    EXPECT_EQ(expected_transform, results_transform);
    EXPECT_EQ(std::size(expected_transform), std::size(trans));
  }

  {
    std::seed_seq seed_expected;
    std::mt19937 eng_expected(seed_expected);
    std::uniform_int_distribution<> gen_expected(-100, 100);
    auto rand_expected = [&gen_expected, &eng_expected]() {
      return gen_expected(eng_expected);
    };
    std::vector<int> expected_random(100);
    std::generate(std::begin(expected_random), std::end(expected_random),
                  rand_expected);
    std::vector<int> results_random(std::begin(random), std::end(random));
    EXPECT_EQ(expected_random, results_random);
    EXPECT_EQ(std::size(expected_random), std::size(random));
  }
}

// test cases that caused problems in the past
TEST_F(HelperTest, BadCases)
{
  // this test case creates an iterator that will get stuck at 0 it not
  // correctly implemented
  auto absolute_bound = RangeFactor<float, Geometric>(1e-6, 1e2, 8);
  auto it = std::begin(absolute_bound);
  auto end = std::end(absolute_bound);
  std::advance(it, 8);
  EXPECT_EQ(end, it);

  RangeFactor a(-100, 100, 5), b(0, 10, 5);
  Builder ab(a, b);
  TransformFactor pl(ab,
                     [](auto&& i) { return std::get<0>(i) + std::get<1>(i); });
  EXPECT_EQ(25, std::size(pl));
}

TEST_F(HelperTest, InvalidIterators)
{
  EXPECT_DEATH({ NormalFactor norm(20, 0, 4); },
               "the standard deviation must be greater than 0");

  EXPECT_DEATH({ RangeFactor rng(100, 3, 100); },
               "the max should be greater than the min");

  EXPECT_DEATH({ RangeFactor rng(1, 2, 100); },
               "the iterator increment should be valid");
}

TEST_F(HelperTest, getParameters)
{
  {
    auto normal = NormalFactor(0, 1, 5);
    auto trans = TransformFactor(normal, [](int i) { return i + 3; });
    auto it = std::begin(trans);
    for (size_t i = 0; i < std::size(trans); ++i, ++it) {
      std::vector<std::size_t> expected{ i };
      {
        auto params = it.get_parameters();
        EXPECT_EQ(expected, params);
      }
      {
        auto params = trans.get_parameters(i);
        EXPECT_EQ(expected, params);
      }
    }
  }
}

TEST_F(HelperTest, getParametersComplex)
{
  std::vector<std::vector<size_t>> expecteds = {
    { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 1, 1 },
    { 2, 1 }, { 0, 2 }, { 1, 2 }, { 2, 2 },
  };
  std::vector<int> a{ 1, 2, 3 };
  std::vector<int> b{ 1, 2, 3 };
  Builder builder(a, b);
  TransformFactor trans(builder, [](auto& tuple) {
    return std::get<0>(tuple) + std::get<1>(tuple);
  });

  for (size_t i = 0; i < std::size(trans); ++i) {
    auto& expected = expecteds.at(i);
    auto params = trans.get_parameters(i);
    EXPECT_EQ(expected, params);
  }
}
