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
    using iterator_category = std::random_access_iterator_tag;
	using index_type = std::array<size_t, std::tuple_size<value_type>::value>;

    iterator(Builder const* builder, bool end_flag):
		builder(builder),
		end_flag(end_flag),
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
		auto e1 = is_endptr();
		auto e2 = it.is_endptr();
		return (e1 && e2) || ( !e1 && !e2 && indices ==  it.indices);
	}

	bool operator!=(iterator const& it) const { return ! (*this == it); }
	bool operator<(iterator const& it) const {
		auto const* cmp = get_real_builder_or_null(it);
		if(cmp != nullptr){
			auto lhs = cmp->to_difference_type(indices, replicant, end_flag);
			auto rhs = cmp->to_difference_type(it.indices, it.replicant, it.end_flag);
			return lhs < rhs;
		} else {
			//two end pointers are equal to each other not less than
			return false;
		}
	}
	bool operator>(iterator const& it) const { return it < *this; }
	bool operator<=(iterator const& it) const { return (*this < it) || (*this == it); }
	bool operator>=(iterator const& it) const { return (it < *this) || (*this == it); }

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
			if(!end_flag)
			{
			value = builder->get_value_at_index(indices);
			}
		}
		return *this;
	}
	iterator& operator--() {
		return *this -= 1;
	}
	iterator operator--(int){
		iterator tmp = *this;
		--(*this);
		return tmp;
	}
    iterator operator++(int){
		iterator tmp = *this;
		++(*this);
		return tmp;
	}
	iterator operator-(difference_type n) const {
		return (*this) + (-n);
	}
	difference_type operator-(iterator const& it) const { 
		auto const*cmp = get_real_builder_or_null(it);
		if(cmp != nullptr){
			auto lhs = cmp->to_difference_type(indices, replicant, end_flag);
			auto rhs = cmp->to_difference_type(it.indices, it.replicant, it.end_flag);
			return lhs - rhs;
		} else { 
			//if both pointers have no builder, they are both end pointers and thus are 0 away
			return 0;
		}
	}
	iterator& operator-=(difference_type n) {
		return (*this) += (-n);
	}
	iterator operator+(difference_type n) const {
		iterator tmp = *this;
		tmp += n;
		return tmp;
	}
	iterator& operator+=(difference_type n) {
		auto pos = builder->to_difference_type(indices, replicant, end_flag);
		builder->from_difference_type(pos+n, indices, replicant, end_flag);
		if(!end_flag)
		{
			value = builder->get_value_at_index(indices); 
		}
		return *this;
	}

	friend std::ostream&
	operator<<(std::ostream& out, iterator const& it) {
		out << "Iterator < builder=" << ((it.builder==nullptr)?("(null)"):("(non-null)")) << " ";
		if(it.end_flag) {
			return out << "END>";
		} else {
			out << "replicant=" << it.replicant << " ";
			out << "indices={";
			auto idx_it = std::begin(it.indices);
			if (idx_it != std::end(it.indices)) {
				out << *idx_it;
				while(idx_it != std::end(it.indices))
				{
					out << ", " << *idx_it;
					++idx_it;
				}
			}
			return out << "}>";
		}
	}

  private:
    Builder const* builder;
	bool end_flag;
	size_t replicant;
	index_type indices;
	value_type value;

	bool is_endptr() const {
		return end_flag || builder == nullptr;
	}

	Builder const* get_real_builder_or_null(iterator const& it) const {
		Builder const* cmp;
		if (builder == nullptr){
			if(it.builder == nullptr){
				assert(end_flag && it.end_flag && "iterators should be end iterators if their builder is set to nullptr");
				return nullptr;
			} else {
				cmp = it.builder;
			}
		} else {
			cmp = builder;
		}
		return cmp;

	}
  };

  using value_type = typename iterator::value_type;

  iterator begin() const { return {this, false}; }

  iterator end() const { return {this, true}; }

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

  typename iterator::difference_type to_difference_type(typename iterator::index_type const& index, size_t const& replicant, bool const end_flag) const {
	  typename iterator::difference_type idx =0;
	  typename iterator::index_type boundries;

	  auto dim_tuple = tuple_transform([](auto&& factor){
			  return std::size(factor);
	  }, factors);
	  auto dim = tuple_to_array<size_t>(dim_tuple);

	  boundries[0] = replicants;
	  for (size_t i = 1; i < index.size(); ++i) {
	  	boundries[i] = dim[i-1] * boundries[i-1];
	  }

	  if(!end_flag){
		  for(size_t i = 0; i < index.size(); ++i) {
			  idx += index[i]* boundries[i];
		  }
		  idx+=replicant;
	  } else {
		  idx = dim.back() * boundries.back();
	  }

	  return idx;
  }

  void from_difference_type(typename iterator::difference_type d, typename iterator::index_type &index, size_t& replicant, bool& end_flag) const { 
	  typename iterator::index_type boundries;

	  auto dim_tuple = tuple_transform([](auto&& factor){
			  return std::size(factor);
	  }, factors);
	  auto dim = tuple_to_array<size_t>(dim_tuple);

	  boundries[0] = replicants;
	  for (size_t i = 1; i < index.size(); ++i) {
	  	boundries[i] = dim[i-1] * boundries[i-1];
	  }
	  if(d < boundries.back() * dim.back()) {
		  for(size_t i = index.size(); i-- > 0;){
			  index[i] = d / boundries[i];
			  d -= (index[i] * boundries[i]);
		  }
		  replicant = d;
	  } else {
		  end_flag = true;
		  std::fill(std::begin(index), std::end(index), 0);
		  replicant = 0;
	  }
  }
  
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
