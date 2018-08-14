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
	std::vector<int> i = {1,2,3,4,5,6,7};
	std::vector<float> f = {1.,2.,3.,4.,5.};
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

namespace {

template<class T>
std::tuple<size_t,size_t> get_sizes(T& builder)
{
	size_t count = std::count_if(std::begin(builder), std::end(builder), [](auto const&){return true;});
	std::set<typename decltype(std::begin(builder))::value_type> unique_cases(std::begin(builder), std::end(builder));
	return std::make_tuple(count, unique_cases.size());
}


}

TEST_F(ParameterSweepBuilder, SizeFullFactorial)
{
	auto [count, unique_count] = get_sizes(example);
	EXPECT_EQ(5*7*30, example.size());
	EXPECT_EQ(5*7*30, std::size(example));
	EXPECT_EQ(5*7, unique_count);
	EXPECT_EQ(5*7*30, count);
}
TEST_F(ParameterSweepBuilder, SizeOneAtATime)
{
	example.set_design(Design::OneAtATime);
	auto [count, unique_count] = get_sizes(example);
	EXPECT_EQ((5+7-1)*30, example.size());
	EXPECT_EQ((5+7-1)*30, std::size(example));
	EXPECT_EQ(5+7-1, unique_count);
	EXPECT_EQ((5+7-1)*30, count);
}

TEST_F(ParameterSweepBuilder, EmptySize)
{
	for(auto const& design : all_designs)
	{
		empty.set_design(design);
		EXPECT_EQ(0,empty.size());
		EXPECT_EQ(0,std::size(empty));

	}
}

namespace {
	template<class T>
	typename std::iterator_traits<T>::difference_type 
	safe_incr(T& it, T end, typename std::iterator_traits<T>::difference_type n = {1}) {
		typename std::iterator_traits<T>::difference_type ret{0};
		while(ret < n && it != end) {
			it++;
			ret++;
		}
		return ret;
	}
	template<class T>
	typename std::iterator_traits<T>::difference_type 
	safe_decr(T& it, T end, typename std::iterator_traits<T>::difference_type n = {1}) {
		typename std::iterator_traits<T>::difference_type ret{0};
		while(ret < n && it != end) {
			it--;
			ret++;
		}
		return ret;
	}


	template<class T>
	typename std::iterator_traits<T>::difference_type
	safe_raccess(T& it, T end, typename std::iterator_traits<T>::difference_type n = {1}) {
		auto distance = end-it;
		typename std::iterator_traits<T>::difference_type ret;
		if(it < end)
			ret = std::min(distance, n);
		else
			ret = std::max(distance, n);
		it += ret;
		return ret;
	}

	
}

TEST_F(ParameterSweepBuilder, RandomAccessCountUp)
{
	example.set_replicants(3);
	auto count = std::size(example);
	auto end = std::end(example);

	for(size_t step = 1; step < count; step++)
	{
		auto beg_raccess = std::begin(example);
		for(size_t i = 0; i < count; i+=step)
		{
			EXPECT_EQ(std::begin(example)+i, beg_raccess);
			EXPECT_EQ(i, beg_raccess.get_id());
			safe_raccess(beg_raccess, end, step);
		}
	}
}

TEST_F(ParameterSweepBuilder, IncrementCountUp)
{
	example.set_replicants(3);
	auto count = std::size(example);
	auto end = std::end(example);

	for(size_t step = 1; step < count; step++)
	{
		auto beg_incr = std::begin(example);
		for(size_t i = 0; i < count; i+=step)
		{
			EXPECT_EQ(i, beg_incr.get_id());
			safe_incr(beg_incr, end, step);
		}
	}
}

TEST_F(ParameterSweepBuilder, RandomAccessCountDown)
{
	example.set_replicants(3);
	auto count = std::size(example);
	auto beg = std::begin(example);

	for(size_t step = 1; step < count; step++)
	{
		auto end_raccess = std::end(example);
		for(size_t i = 0; i < count; i+=step)
		{
			EXPECT_EQ(std::end(example)-i, end_raccess);
			EXPECT_EQ(count-i, end_raccess.get_id());
			safe_raccess(end_raccess, beg, -step);
		}
	}

}
TEST_F(ParameterSweepBuilder, IncrementCountDown)
{
	example.set_replicants(3);
	auto count = std::size(example);
	auto beg = std::begin(example);

	for(size_t step = 1; step < count; step++)
	{
		std::cout << "starting step " << step << std::endl;
		auto end_incr = std::end(example);
		for(size_t i = 0; i < count; i+=step)
		{
			EXPECT_EQ(std::end(example)-i, end_incr);
			EXPECT_EQ(count-i, end_incr.get_id());
			safe_decr(end_incr, beg, step);
		}
	}

}
