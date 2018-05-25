/*
 * some code to prototype computing offsets
 */
#include <iostream>
#include <functional>
#include <numeric>
#include <vector>
#include <iterator>
#include <algorithm>

template <class T>
struct Range
{
	Range(T const& i): first(std::cbegin(i)), last(std::cend(i)) {}
	typename T::const_iterator first;
	typename T::const_iterator last;
};

template <class CharT, class Traits, class T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits>& out, Range<T> const& range) 
{
	auto first = range.first;
	auto last = range.last;

	out << "{";
	if(first != last)
	{
		out << *first;
		++first;
	}
	while(first != last)
	{
		out << ", " << *first;
		++first;
	}
	out << "}";
	return out;
}


template<class Cont1, class Cont2>
void inefficent(Cont1 const& dim, Cont2 & results)
{
	std::partial_sum(std::rbegin(dim), std::rend(dim), std::rbegin(results), std::multiplies<int>{});
	std::rotate(std::begin(results),std::begin(results)+1, std::end(results));
	results.back() = 1;
}

template <class Cont1, class Cont2>
void compute_offsets(Cont1 const& dim, Cont2& results)
{
	std::multiplies<int> op{};
	auto first = std::rbegin(dim);
	auto last  = std::rend(dim);
	auto out = std::rbegin(results);
	typename decltype(out)::value_type sum = 1;
	*out++ = sum;
	while(std::next(first) != last)
	{
		sum = op(std::move(sum), *first++);
		*out++ = sum;
	}
}

int main(int argc, char *argv[])
{
	constexpr std::array<int,3> dim = {7,5,13};
	std::vector<int> results(3);
	
	std::cout << "base " << Range(dim) << std::endl;

	//inefficent(dim,results);
	auto desired = {dim[2]*dim[1], dim[2], 1};
	std::cout << "desired " << Range(desired) << std::endl;
	compute_offsets(dim, results);
	std::cout << "offsets " << Range(results) << std::endl;

	int elms = std::accumulate(std::cbegin(dim), std::cend(dim), 1, std::multiplies<int>{});
	int j[dim[0]][dim[1]][dim[2]];
	std::iota((int*)j, (int*)j+elms, 0);
	constexpr std::array<int,3> indexs = {1, 4, 2};

	std::array<int,3> offsets;

	auto index = std::inner_product(std::begin(indexs), std::end(indexs), std::begin(offsets), 0);

	std::cout << "j+6 " << *(((int*)j)+(indexs[0]*(results[0]) + (results[1])*indexs[1] + results[2]*indexs[2])) << std::endl;
	std::cout << "j+6 " << *(((int*)j)+index) << std::endl;
	std::cout << "j+6 " << j[indexs[0]][indexs[1]][indexs[2]] << std::endl;

	return 0;
}
