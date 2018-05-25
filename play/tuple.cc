/*
 * Some code to track down why set was not deducing the key type for tuples
 * It boils down to GCC 7.3.0-r1 doesn't have them in libstdc++ until 8.1.0
 */
#include <list>
#include <set>
#include <tuple>
#include <iterator>
#include <vector>
#include <iomanip>
#include <iostream> 
#include <algorithm>

class custom_iterator
{
	public:
		using vi_t = std::vector<std::tuple<int,int,int>>::iterator;
		using value_type = std::tuple<int,int,int>; 
		using reference = value_type&; 
		using pointer = value_type*; 
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		~custom_iterator()=default;

		custom_iterator()=default;
		custom_iterator(vi_t const& it): inner(it){}

		custom_iterator(custom_iterator const& it): inner(it.inner){}

		custom_iterator& operator=(custom_iterator const& it){
			inner = it.inner;
			return *this;
		}

		bool operator==(custom_iterator const& it) const { return inner == it.inner; }

		bool operator!=(custom_iterator const& it) const { return ! (it == *this); }

		reference operator*() {
			return *inner;
		}

		reference operator->() {
			return *inner;
		}

		custom_iterator& operator++() {
			inner++;
			return *this;
		}

		custom_iterator operator++(int) {
			custom_iterator tmp = *this;
			++(*this);
			return tmp;
		}


	private:
		std::vector<std::tuple<int,int,int>>::iterator inner;

};
int main(int argc, char *argv[])
{
	std::tuple<int, int, int> t1 = {1,2,3};
	std::tuple<int, int, int> t2[] = {{1,2,3},{2,3,4}};
	std::vector<std::tuple<int,int,int>> t3 ={{1,2,3},{2,3,4}}; 
	std::list<std::tuple<int,int,int>> t4 ={{1,2,3},{2,3,4}}; 

	std::set cases = {t1};
	std::set cases2 = {t2};
	std::set cases3 {std::begin(t2), std::end(t2)};
	std::set cases4 {std::begin(t3), std::end(t3)};

	//doesn't deduce key
	//std::set cases5 (std::begin(t4), std::end(t4));
	//std::set<std::tuple<int,int,int>> cases5 (std::begin(t4), std::end(t4));

	custom_iterator i1{std::begin(t3)};
	custom_iterator i2{std::end(t3)};
	std::count_if(i1,i2, [](auto const& i){ return true;});
	std::set<std::tuple<int,int,int>> unique_e(i1,i2);

	std::cout << std::boolalpha;
	std::cout << (t3[0] < t3[1]) << std::endl;
	return 0;
}
