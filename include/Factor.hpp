#include <numeric>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <tuple>


#include <tuple_algorithms.hpp>
namespace ParameterSweep {

template <class... Containers>
class Factor
{
public:
	Factor(Containers... containers)
		: containers(std::forward_as_tuple(containers...))
	{}

  class iterator
  {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::conditional_t<
      std::tuple_size<std::tuple<Containers...>>::value != 0,
      typename std::tuple_element<0, std::tuple<Containers...>>::type::value_type,
      std::tuple<>>;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::forward_iterator_tag;
	struct index_type {
		index_type(): container_index(0), element_index(0) {}
		bool operator==(const index_type& rhs) const { return container_index == rhs.container_index && element_index == rhs.element_index;}
		size_t container_index;
		size_t element_index;
	};

    iterator()
      : factor(nullptr)
      , end_flag(true)
      , index()
      , value()
    {}
    iterator(Factor* factor)
      : factor(factor)
      , end_flag(false)
      , index()
      , value()
    {factor->get_value(index, value);}
    iterator(iterator const& it) = default;
    iterator& operator=(iterator const& it) = default;

    void swap(iterator& it)
    {
      std::swap(end_flag, it.end_flag);
      std::swap(factor, it.factor);
      std::swap(index, it.index);
      std::swap(value, it.value);
    }

    reference operator->() { return value; }

    reference operator*() { return value; }

    iterator& operator++()
    {
      assert(factor != nullptr &&
             "cannot increment a default constructed iterator");

      factor->next_index(index, end_flag);
      factor->get_value(index, value);

      return *this;
    }

    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }


    bool operator==(iterator const& it) const
    {
      return end_flag == it.end_flag || (factor == it.factor && index == it.index);
    }

    bool operator!=(iterator const& it) const { return !(*this == it); }

  private:
    Factor* factor;
    bool end_flag;
    index_type index;
    value_type value;
  };

  iterator begin() { return iterator(this); }

  iterator end() { return iterator(); }

  size_t size() const { 
	  auto sizes = tuple_transform([](auto& container) {return std::size(container);}, containers);
	  auto sizes_a = tuple_to_array<size_t>(sizes);
	  return std::accumulate(std::begin(sizes_a), std::end(sizes_a), 0);
  }

private:
  std::tuple<Containers...> containers;

  void next_index(typename iterator::index_type& index, bool& end_flag) {
	  size_t size_current_container;
	  apply_to_elm([&size_current_container](auto& container){ size_current_container = std::size(container); }, containers, index.container_index);
	  if(++index.element_index >= size_current_container)
	  {
		  ++index.container_index;
		  index.element_index = 0;
		  if(index.container_index == std::tuple_size<std::tuple<Containers...>>::value){
			  end_flag = true;
			  index.container_index = 0;
		  }
		  else {end_flag = false;}
	  }
  }

  void get_value(typename iterator::index_type& index, typename iterator::reference value)
  {
	  size_t& container_index = index.container_index;
	  size_t& element_index = index.element_index;
	  apply_to_elm([&element_index, &value](auto&& container){value = *std::next(std::begin(container), element_index);}, containers, container_index);
  }
};

template <class... Types>
Factor(Types... containers)->Factor<Types...>;

} // namespace ParameterSweep
