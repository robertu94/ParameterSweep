#include "tuple_algorithms.hpp"

#include <gtest/gtest.h>

TEST(tuple_algorithms, lenghts)
{
  auto begin_end = [](auto&& c) { return std::make_tuple(std::begin(c), std::end(c)); };
  auto length = [](auto&& be) { return std::distance(std::get<0>(be), std::get<1>(be));};
  std::vector<int> i = {1,2,3};
  std::vector<float> f = {1.1, 2.1, 3.1, 4.1};

  auto t = std::make_tuple(i,f);
  auto t2 = tuple_transform(begin_end, t);
  auto t3 = tuple_transform(length, t2);
  auto t3_expected = std::make_tuple(3,4);
  ASSERT_EQ(t3_expected, t3);
}

TEST(tuple_algorithms, find)
{

  auto begin_end = [](auto&& c) { return std::make_tuple(std::begin(c), std::end(c)); };
  auto find_t = [](auto&& i, auto&& j) { return std::find(std::get<0>(i), std::get<1>(i),j); };
  auto not_equals = [](auto&&i, auto&&j){return i != j;};
  auto deref = [](auto&&i){return *i;};

  std::vector<int> i = {1,2,3};
  std::vector<float> f = {1.1, 2.1, 3.1, 4.1};

  auto t = std::make_tuple(i,f);
  auto t2 = tuple_transform(begin_end, t);
  auto t4 = std::make_tuple(3, 4.1f);
  auto t5 = tuple_transform(find_t, t2, t4);
  auto t8_expected = std::make_tuple(
		  *std::find(std::begin(i),std::end(i), 3),
		  *std::find(std::begin(f),std::end(f), 4.1f)
		  );
  auto t8 = tuple_transform(deref,t5);
  EXPECT_EQ(t8_expected, t8);

  auto t6 = std::make_tuple(std::end(i), std::end(f));
  auto t7 = tuple_transform(not_equals, t5, t6);
  auto t7_expected = std::make_tuple(true, true);
  EXPECT_EQ(t7_expected, t7);
}

TEST(tuple_algorithms, to_array)
{
	auto t1 = std::make_tuple(1,2,3);
	std::array<int,3> t1_expected = {1,2,3};
	EXPECT_EQ(t1_expected, tuple_to_array<int>(t1));
}

TEST(tuple_algorithms, to_tuple)
{
	std::array<int,3> t1 = {1,2,3};
	auto t1_expected = std::make_tuple(1,2,3);
	EXPECT_EQ(t1_expected, array_to_tuple(t1));
}

TEST(tuple_algorithms, empty)
{
	size_t count = 0;
	auto empty = std::make_tuple();
	auto t1 = tuple_transform([&count](auto &&){ return count++; }, empty);
	EXPECT_EQ(empty, t1);
	EXPECT_EQ(0, count);

}

TEST(tuple_algorithms, apply_to_elm)
{
	auto t = std::make_tuple(1,2,3);
	size_t ret;
	apply_to_elm([&ret](auto &i){ret= i+3;}, t, 0);
	EXPECT_EQ(4, ret);
	apply_to_elm([&ret](auto &i){ret= i+3;}, t, 1);
	EXPECT_EQ(5, ret);
	apply_to_elm([&ret](auto &i){ret = i+3;}, t, 2);
	EXPECT_EQ(6, ret);
}


TEST(tuple_algorithms, get_runtime)
{
	auto t = std::make_tuple(1,2,3);
	size_t ret;
	for (size_t i = 1; i < 4; ++i) {
		get_runtime(t,i,ret);
		EXPECT_EQ(i,ret);
	}


}
