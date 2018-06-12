#include <iterator_checks.hpp>
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
	using iterator = example_type::iterator;
};

TEST_F(ParameterSweepBuilder, IteratorPrimitives)
{
	TestForwardWritable<std::tuple<int,float>, example_type::iterator> tests;
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

TEST_F(ParameterSweepBuilder, DebugIteratorPrinter)
{
	example.set_replicants(2);
	struct {
		iterator it;
		std::string expected;
	} test_cases[] = {
		/*0*/{std::begin(example), std::string("Iterator < builder=(non-null) replicant=0 indices={0, 0, 0}>")},
		/*1*/{std::next(std::begin(example)), std::string("Iterator < builder=(non-null) replicant=1 indices={0, 0, 0}>")},
		/*2*/{((++(std::begin(example)))), std::string("Iterator < builder=(non-null) replicant=1 indices={0, 0, 0}>")},
		/*3*/{(++(++(std::begin(example)))), std::string("Iterator < builder=(non-null) replicant=0 indices={1, 1, 0}>")},
		/*4*/{(std::begin(example)+2), std::string("Iterator < builder=(non-null) replicant=0 indices={1, 1, 0}>")},
		/*5*/{std::end(example), std::string("Iterator < builder=(non-null) END>")},
		/*6*/{iterator(), std::string("Iterator < builder=(null) END>")},
	};
	auto i = 0;
	for(auto const& test_case: test_cases) {
		std::stringstream ss;
		ss << test_case.it;
		EXPECT_EQ(test_case.expected, ss.str()) << "test case " << i << " failed";
		++i;
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

TEST_F(ParameterSweepBuilder, RandomAccess)
{
	example.set_replicants(3);

	/*
	//count up to the end
	auto count = std::size(example);
	auto it = std::begin(example);
	auto end_ptr_expected = std::end(example);
	for(size_t i = 0; i < count; ++i, ++it){
		auto to_end = count - i;
		auto end_ptr = it + to_end;
		EXPECT_EQ(end_ptr_expected, end_ptr);
	}


	//count down from the end
	auto beg_ptr_expected = std::begin(example);
	it = std::end(example);
	for(size_t i = 0; i < count; ++it, ++i){
		auto to_beginning = count - i;
		auto beg_ptr  = it - to_beginning;
		EXPECT_EQ(beg_ptr_expected, beg_ptr);
	}
	*/

}

