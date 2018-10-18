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
			iterator(RandomOrder const* ptr, bool is_end=false):
				ptr(ptr),
				index(is_end?ptr->size():0),
				value()
		{
			if(!is_end) update();
		}

			value_type operator*() const{
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

			auto get_parameters() const {
				return (ptr->first()+mapped_index()).get_parameters();
			}

			auto get_id() const {
				return index;
			}

		private:

				size_t mapped_index() const {
				return (A + (B * index)) % ptr->size();
				}

			void update() {
				value = *std::next(ptr->first(), mapped_index());
			}
		RandomOrder const* ptr;
		size_t index;
		value_type value;
	};


	iterator begin() const {
		return iterator(this);
	}

	iterator end() const {
		return iterator(this, true);
	}

	size_t size() const {
		return std::size(container);
	}
	auto levels() const {
		return container.levels();
	}



	auto get_parameters(size_t i) const {
		auto it = std::advance(begin(),i);
		return it.get_parameters();
	}

	private:

	auto first() const {
		return std::begin(container);
	}
	
	
		T container;
};

//the choice of 223 and 227 is arbitrary, they just need to be co-prime
template <class T>
using RandomOrderA = RandomOrder<T,223,227>;
