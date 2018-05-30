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
		//empty = Factor{};

		example = Factor(vi, si);
	}

	std::vector<int> vi = {1,2,3,4};
	std::set<int> si = {1,2,5};
	using example_type = Factor<std::vector<int>, std::set<int>>;
	//Factor<> empty;
	example_type example = Factor(vi,si);
};

TEST_F(FactorTest, IteratorPrimitives)
{
	typedef example_type::iterator iterator;
	static_assert(std::is_same<iterator::value_type, int>::value, "iterator value type incorrect");
	static_assert(std::is_same<iterator::reference, int&>::value, "iterator reference type incorrect");
	static_assert(std::is_same<iterator::pointer, int*>::value, "iterator pointer type incorrect");
	static_assert(std::is_same<iterator::difference_type, std::ptrdiff_t>::value, "iterator difference_type type incorrect");
	static_assert(std::is_same<iterator::iterator_category, std::forward_iterator_tag>::value, "iterator category type incorrect");

	static_assert(std::is_copy_constructible<iterator>::value, "is not copy constructible");
	//is copy assignable
	{
	iterator a, b;
	a = b;
	}

	static_assert(std::is_destructible<iterator>::value, "is not destructable");

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
}

TEST_F(FactorTest, Size)
{
	auto get_sizes = [](auto& builder)
	{
		size_t count = std::count_if(std::begin(builder), std::end(builder), [](auto const&){return true;});
		std::vector<int> values(std::begin(builder), std::end(builder));
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

	{
		//auto [count, values] = get_sizes(empty);
		//std::vector<int> expected = {};
		//EXPECT_EQ(0, empty.size());
		//EXPECT_EQ(0, std::size(empty));
		//EXPECT_EQ(0, count);
		//EXPECT_EQ(expected, values);
	}

}
