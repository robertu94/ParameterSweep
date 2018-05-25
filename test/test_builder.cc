#include <ParameterSweep.hpp>

#include <string>
#include <sstream>
#include <iterator>
#include <type_traits>
#include <gtest/gtest.h>

using namespace ParameterSweep;

class ParameterSweepBuilder: public ::testing::Test
{
	public:
	virtual void SetUp() {
		empty = Builder{};

		example =  Builder(i,f);
		example.set_replicants(30);
	}

	Builder<> empty{};
	std::vector<int> i = {1,2,3};
	std::vector<float> f = {1.,2.,3.};
	Builder<std::vector<int>,std::vector<float>> example = {i,f};

	static constexpr auto all_designs = {Design::FullFactorial, Design::OneAtATime};
	using example_type = Builder<std::vector<int>,std::vector<float>>;
};

TEST_F(ParameterSweepBuilder, IteratorPrimitives)
{
	typedef example_type::iterator iterator;
	static_assert(std::is_same<iterator::value_type, std::tuple<int,float>>::value, "iterator value type incorrect");
	static_assert(std::is_same<iterator::reference, std::tuple<int,float>&>::value, "iterator reference type incorrect");
	static_assert(std::is_same<iterator::pointer, std::tuple<int,float>*>::value, "iterator pointer type incorrect");
	static_assert(std::is_same<iterator::difference_type, std::ptrdiff_t>::value, "iterator difference_type type incorrect");
	static_assert(std::is_same<iterator::iterator_category, std::forward_iterator_tag>::value, "iterator category type incorrect");


	//iterator_traits typedefs exist
	static_assert(std::is_same<std::iterator_traits<iterator>::value_type, std::tuple<int,float>>::value, "iterator value type incorrect");
	static_assert(std::is_same<std::iterator_traits<iterator>::reference, std::tuple<int,float>&>::value, "iterator reference type incorrect");
	static_assert(std::is_same<std::iterator_traits<iterator>::pointer, std::tuple<int,float>*>::value, "iterator pointer type incorrect");
	static_assert(std::is_same<std::iterator_traits<iterator>::difference_type, std::ptrdiff_t>::value, "iterator difference_type type incorrect");
	static_assert(std::is_same<std::iterator_traits<iterator>::iterator_category, std::forward_iterator_tag>::value, "iterator category type incorrect");

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

TEST_F(ParameterSweepBuilder, DebugPrinting)
{
  struct
  {
    Design design;
    std::string expected;
  } test_cases[] = {
    { Design::FullFactorial,
      std::string("ParameterSweep replicants: 30, design: full factorial, "
                  "order: default, factors: (2)") },
    { Design::OneAtATime,
      std::string("ParameterSweep replicants: 30, design: one at a time, "
                  "order: default, factors: (2)") }
  };

  for (auto const& test_case : test_cases) {
    example.set_design(test_case.design);
    std::stringstream ss;
    ss << example;
    EXPECT_EQ(test_case.expected, ss.str());
  }
}

TEST_F(ParameterSweepBuilder, Size)
{
	auto get_sizes = [](auto& builder)
	{
		size_t count = std::count_if(std::begin(builder), std::end(builder), [](auto const&){return true;});
		std::set<typename decltype(std::begin(builder))::value_type> unique_cases(std::begin(builder), std::end(builder));
		return std::make_tuple(count, unique_cases.size());
	};

	{
		auto [count, unique_count] = get_sizes(example);
		EXPECT_EQ(3*3*30, example.size());
		EXPECT_EQ(3*3*30, std::size(example));
		EXPECT_EQ(3*3, unique_count);
		EXPECT_EQ(3*3*30, count);
	}

	{
		example.set_design(Design::OneAtATime);
		auto [count, unique_count] = get_sizes(example);
		EXPECT_EQ((3+3-1)*30, example.size());
		EXPECT_EQ((3+3-1)*30, std::size(example));
		EXPECT_EQ(3+3-1, unique_count);
		EXPECT_EQ((3+3-1)*30, count);
	}

	for(auto const& design : all_designs)
	{
		empty.set_design(design);
		//size_t count = 0;
		//for(auto& config: builder)
		//{
		//	count++;
		//}
		//ASSERT_EQ(0,count);
		EXPECT_EQ(0,empty.size());
		EXPECT_EQ(0,std::size(empty));

	}
}
