#pragma once
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <tuple>
#include <utility>
#include <cassert>

#include <tuple_algorithms.hpp>

namespace ParameterSweep {

enum class Order
{
  Default,
  Sorted
};
std::ostream& operator<<(std::ostream&, Order const&);

enum class Design
{
  FullFactorial,
  OneAtATime
};
std::ostream& operator<<(std::ostream&, Design const&);

template <class... Factors>
class Builder
{
public:
  Builder(Factors... factors)
    : order(Order::Default)
    , design(Design::FullFactorial)
    , replicants(1)
    , factors(std::forward_as_tuple(factors...))
  {}

  class iterator
  {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<typename Factors::value_type...>;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::forward_iterator_tag;
	using index_type = std::array<size_t, std::tuple_size<value_type>::value>;

    iterator(Builder const* builder):
		builder(builder),
		end_flag(false),
		replicant(0),
		indices(),
		value(builder->get_value_at_index(indices))
    {}

    // forward iterator
	iterator():
		builder(nullptr),
		end_flag(true),
		replicant(),
		indices(),
		value()
	  {}
	
    // copy consructable
    iterator(iterator const& it)=default;
    // copy assignable
    iterator& operator=(iterator const& it)=default;
	// swappable
    void swap(iterator& it){
		std::swap(builder, it.builder);
		std::swap(end_flag, it.end_flag);
		std::swap(replicant, it.replicant);
		std::swap(value, it.value);
		std::swap(indices, it.indices);
	}
    // equality comparable
    bool operator==(iterator const& it) const {
		//make all end pointers equal
		return end_flag == it.end_flag || ((builder == it.builder) && \
			   (value == it.value) && \
			   indices == it.indices);
	}
	bool operator!=(iterator const& it) const { return ! (*this == it); }

    // iterator
    reference operator->(){
		return value;
	}
    reference operator*(){
		return value;
	}
    iterator& operator++(){
		assert(builder != nullptr && "cannot increment a default constructed iterator");
		replicant++;
		if(replicant >= builder->replicants){
			replicant = 0;
			builder->next_index(indices, end_flag);
			value = builder->get_value_at_index(indices);
		}
		return *this;
	}
    iterator operator++(int){
		iterator tmp = *this;
		++(*this);
		return tmp;
	}

  private:
    Builder const* builder;
	bool end_flag;
	size_t replicant;
	index_type indices;
	value_type value;
  };

  using value_type = typename iterator::value_type;

  iterator begin() const { return {this}; }

  iterator end() const { return {}; }

  size_t size() const
  {
    auto sizes =
      tuple_transform([](auto&& factor) { return std::size(factor); }, factors);
    auto sizes_array = tuple_to_array<size_t>(sizes);
	if (std::size(sizes_array) != 0)
	{
    switch (design) {
      case Design::FullFactorial:
        return replicants * std::accumulate(std::begin(sizes_array),
                                            std::end(sizes_array), 1,
                                            std::multiplies<size_t>{});
      case Design::OneAtATime:
        return replicants *
               (std::accumulate(std::begin(sizes_array), std::end(sizes_array),
                                0, std::plus<size_t>{}) -
                (std::size(sizes_array) - 1));
      default:
        throw std::runtime_error{ "invalid design" };
    }
	} else {
		//if there are no factors, there is nothing to test
		return 0;
	}
  }

  Builder& set_order(Order const& order)
  {
    this->order = order;
    return *this;
  }

  Builder& set_design(Design const& design)
  {
    this->design = design;
    return *this;
  }

  Builder& set_replicants(size_t replicants)
  {
    this->replicants = replicants;
    return *this;
  }

  std::ostream& operator<<(std::ostream& out) const
  {
    return out << "ParameterSweep replicants: " << replicants
               << ", design: " << design << ", order: " << order
               << ", factors: ("
               << std::tuple_size<std::tuple<Factors...>>::value << ")";
  }

private:
  Order order;
  Design design;
  size_t replicants;
  std::tuple<Factors...> factors;

  void next_index(typename iterator::index_type& index, bool& end_flag) const
  {
	  auto dim_tuple = tuple_transform([](auto&& factor){
			  return std::size(factor);
	  }, factors);
	  auto dim = tuple_to_array<size_t>(dim_tuple);
	  size_t i = 0;
	  switch(design){
		  case Design::FullFactorial:
			  while(i < dim.size() && ++index[i] >= dim[i])
			  {
				  index[i++] = 0;
			  }
			  end_flag = (i==dim.size());
			  break;
		  case Design::OneAtATime:
			  for(; i < dim.size(); ++i) { if(index[i] != 0) break; }
			  if(i == dim.size())
			  {
				  ++index[0];
				  end_flag = false;
			  } else {
				  if(++index[i] >= dim[i]) 
				  {
					  index[i++] = 0;
					  if(i < dim.size())
					  {
						  ++index[i];
						  end_flag = false;
					  } else {
						  end_flag = true;
					  }
				  }
			  }
			  break;
	  }
  }

  typename iterator::value_type
  get_value_at_index(typename iterator::index_type const& index_a) const{
	  auto index = array_to_tuple(index_a);
	  auto values = tuple_transform([](auto&& factor, auto const& index){
	      return *std::next(std::begin(factor), index);
	  }, factors, index);
	  return values;
  }
};
	  

template <class... Factors>
std::ostream&
operator<<(std::ostream& out, Builder<Factors...> const& builder)
{
  return builder << out;
}

template <class... Factors>
Builder(Factors... factors)->Builder<Factors...>;

} /*end namespace ParameterSweep*/
