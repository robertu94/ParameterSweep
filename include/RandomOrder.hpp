#pragma once
#include<cstddef>
#include<tuple>
#include<iterator>
#include <numeric>
#include <cassert>

template <class T, size_t A, size_t B>
class RandomOrder 
{

	public:
		RandomOrder(T container): container(container)
	{
		auto size = std::size(container);
		assert(std::lcm(A,size) ==A * size);
		assert(std::lcm(B,size) == B * size);
		static_assert(std::lcm(A,B) == A*B, "A and B must be co-prime");
	}

	using params_type = size_t;
	class iterator
	{
			using iterator_ = typename T::iterator;
		public:
			using value_type = typename std::iterator_traits<iterator_>::value_type;
			using pointer = typename std::iterator_traits<iterator_>::pointer;
			using reference = typename std::iterator_traits<iterator_>::reference;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::random_access_iterator_tag;

			iterator();
			iterator(iterator_ it, params_type t, bool is_end=false):
				params(t),
				index(is_end?t:0),
				it(it),
				value()
		{
			if(!is_end) update();
		}

			reference operator*() {
				return value;
			}
			pointer operator->() const {
				return &value;
			}

			iterator& operator++() {
				index++;
				update();
				return *this;
			}
			iterator operator++(int) {
				iterator tmp = *this;
				++*this;
				return tmp;
			}
			iterator& operator--() {
				index--;
				return *this;
			}
			iterator operator--(int) {
				iterator tmp = *this;
				--*this;
				return tmp;
			}

			iterator& operator+=(difference_type n){
				index += n;
				update();
				return *this;
			}
			iterator operator+ (difference_type n) const {
				iterator tmp = *this;
				return tmp+=n;

			}
			iterator operator- (difference_type n) const {
				iterator tmp = *this;
				tmp+=(-n);
				return tmp;
			}
			iterator& operator-=(difference_type n) {
				*this+=(-n);
				return *this;
			}

			value_type operator[](difference_type n) const {
				iterator tmp = *this;
				tmp+=n;
				return *tmp;
			}

			difference_type operator-(iterator const& rhs) {
				return index - rhs.index;
			}

			bool operator<(iterator const& rhs) const {
				return index < rhs.index;
			}
			bool operator>(iterator const& rhs) const {
				return index > rhs.index;
			}
			bool operator>=(iterator const& rhs) const {
				return index >= rhs.index;
			}
			bool operator<=(iterator const& rhs) const {
				return index <= rhs.index;
			}
			bool operator==(iterator const& rhs) const {
				return index == rhs.index;
			}
			bool operator!=(iterator const& rhs) const {
				return index != rhs.index;
			}

		private:
			void update() {
				auto mapped_index = (A + (B * index)) % params;
				value = *std::next(it, mapped_index);
			}
		params_type params;
		size_t index;
		iterator_ it;
		value_type value;
	};


	iterator begin() {
		return iterator(std::begin(container), std::size(container));
	}

	iterator end() {
		return iterator(std::end(container), std::size(container), true);
	}

	private:
	
		T container;
};

//the choice of 223 and 227 is arbitrary, they just need to be co-prime
template <class T>
using RandomOrderA = RandomOrder<T,223,227>;
