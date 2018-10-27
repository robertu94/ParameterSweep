#pragma once
#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <tuple>
#include <vector>

#include "tuple_algorithms.hpp"
#include "type_traits.hpp"

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
    using value_type =
      std::conditional_t<std::tuple_size<std::tuple<Containers...>>::value != 0,
                         typename std::tuple_element<
                           0, std::tuple<Containers...>>::type::value_type,
                         std::tuple<>>;
    using reference = value_type&;
    using const_reference = value_type const&;
    using pointer = value_type*;
    using iterator_category = std::random_access_iterator_tag;
    struct index_type
    {
      index_type()
        : container_index(0)
        , element_index(0)
      {}
      bool operator==(const index_type& rhs) const
      {
        return container_index == rhs.container_index &&
               element_index == rhs.element_index;
      }
      size_t container_index;
      size_t element_index;
    };

    iterator()
      : factor(nullptr)
      , end_flag(true)
      , index()
      , value()
    {}
    iterator(Factor const* factor)
      : factor(factor)
      , end_flag(false)
      , index()
      , value()
    {
      factor->get_value(index, value);
    }
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
    const_reference operator->() const { return value; }

    reference operator*() { return value; }
    const_reference operator*() const { return value; }

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

		iterator operator--(int)
		{
      iterator tmp = *this;
      (*this) -= 1;
      return tmp;
		}

		iterator& operator--()
		{
      (*this) -= 1;
			return *this;
		}

    difference_type operator-(iterator const& ptr) const
    {
      auto const* cmp = get_real_factor_or_null(ptr);
      if (cmp != nullptr) {
        auto pos_lhs = cmp->to_difference_type(index, end_flag);
        auto pos_rhs = cmp->to_difference_type(ptr.index, ptr.end_flag);
        return pos_lhs - pos_rhs;
      } else {
        return 0;
      }
    }

    iterator operator+(difference_type n) const
    {
      iterator tmp = *this;
      tmp += n;
      return tmp;
    }
    iterator operator-(difference_type n) const
    {
      iterator tmp = *this;
      tmp += (-n);
      return tmp;
    }

    iterator& operator-=(difference_type n)
    {
      *this += (-n);
      return *this;
    }
    iterator& operator+=(difference_type n)
    {
      auto pos = factor->to_difference_type(index, end_flag);
      factor->from_difference_type(pos + n, index, end_flag);
      if (!end_flag) {
        factor->get_value(index, value);
      }
      return *this;
    }
    bool operator<(iterator const& rhs) const
    {
      auto const* cmp = get_real_factor_or_null(rhs.factor);
      if (factor != nullptr) {
        auto pos_lhs = cmp->to_difference_type(index, end_flag);
        auto pos_rhs = cmp->to_difference_type(rhs.index, rhs.end_flag);
        return pos_lhs < pos_rhs;
      } else {
        return false;
      }
    }
    bool operator>(iterator const& rhs) const { return rhs < *this; }
    bool operator>=(iterator const& rhs) const { return !(*this < rhs); }
    bool operator<=(iterator const& rhs) const { return !(*this > rhs); }

    bool operator==(iterator const& it) const
    {
      return end_flag == it.end_flag ||
             (factor == it.factor && index == it.index);
    }

    bool operator!=(iterator const& it) const { return !(*this == it); }

    std::vector<size_t> get_parameters() const
    {
      difference_type d = factor->to_difference_type(index, end_flag);
      return factor->get_parameters(d);
    }

  private:
    Factor const* get_real_factor_or_null(iterator const& rhs) const
    {
      Factor const* cmp;
      if (factor == nullptr) {
        if (rhs.factor == nullptr) {
          assert(end_flag && rhs.end_flag &&
                 "iterators should be end iterators if their factors are set "
                 "to nullptr");
          cmp = nullptr;
        } else {
          cmp = rhs.factor;
        }
      } else {
        cmp = factor;
      }
      return cmp;
    }

    Factor const* factor;
    bool end_flag;
    index_type index;
    value_type value;
  };
  using value_type = typename iterator::value_type;

  iterator begin() const { return iterator(this); }

  iterator end() const { return iterator(); }

  size_t size() const
  {
    auto sizes = tuple_transform(
      [](auto& container) { return std::size(container); }, containers);
    auto sizes_a = tuple_to_array<size_t>(sizes);
    return std::accumulate(std::begin(sizes_a), std::end(sizes_a), 0);
  }

  std::vector<size_t> get_parameters(size_t d) const
  {
    typename iterator::index_type index;
    bool end_flag;
    from_difference_type(d, index, end_flag);

    std::array<size_t, std::tuple_size<std::tuple<Containers...>>::value>
      multi_index{};
    multi_index[index.container_index] = index.element_index;
    auto multi_index_tuple = array_to_tuple(multi_index);

    auto params_t = tuple_transform(
      [](auto const& index, auto const& container) -> std::vector<size_t> {
        if constexpr (is_builder_v<std::decay_t<decltype(container)>>) {
          return container.get_parameters(index);
        } else {
          return std::vector<size_t>{ index };
        }
      },
      multi_index_tuple, containers);
    auto params_a = tuple_to_array<std::vector<size_t>>(params_t);

    std::vector<size_t> params_v;
    params_v.push_back(index.container_index);
    for (auto const& param : params_a) {
      for (auto const& entry : param) {
        params_v.push_back(entry);
      }
    }
    return params_v;
  }

private:
  std::tuple<Containers...> containers;

  typename iterator::difference_type to_difference_type(
    typename iterator::index_type const& index, bool const end_flag) const
  {
    auto sizes = tuple_transform(
      [](auto const& container) { return std::size(container); }, containers);
    auto boundries = tuple_to_array<std::size_t>(sizes);
    std::partial_sum(std::begin(boundries), std::end(boundries),
                     std::begin(boundries));
    if (!end_flag) {
      auto last_boundry =
        (index.container_index == 0) ? 0 : boundries[index.container_index - 1];
      return last_boundry + index.element_index;
    } else {
      return boundries.back();
    }
  }

  void from_difference_type(typename iterator::difference_type d,
                            typename iterator::index_type& index,
                            bool& end_flag) const
  {
    auto sizes = tuple_transform(
      [](auto const& container) { return std::size(container); }, containers);
    auto boundries = tuple_to_array<std::size_t>(sizes);
    std::partial_sum(std::begin(boundries), std::end(boundries),
                     std::begin(boundries));

    end_flag = (static_cast<size_t>(d) == boundries.back());
    auto container =
      std::upper_bound(std::begin(boundries), std::end(boundries), d);
    auto container_idx = std::distance(std::begin(boundries), container);

    index.container_index = container_idx;
    if (container_idx == 0) {
      index.element_index = d;
    } else {
      index.element_index = d - boundries[index.container_index - 1];
    }
  }

  void next_index(typename iterator::index_type& index, bool& end_flag) const
  {
    size_t size_current_container;
    apply_to_elm(
      [&size_current_container](auto& container) {
        size_current_container = std::size(container);
      },
      containers, index.container_index);
    if (++index.element_index >= size_current_container) {
      ++index.container_index;
      index.element_index = 0;
      if (index.container_index ==
          std::tuple_size<std::tuple<Containers...>>::value) {
        end_flag = true;
        index.container_index = 0;
      } else {
        end_flag = false;
      }
    }
  }

  void get_value(typename iterator::index_type& index,
                 typename iterator::reference value) const
  {
    size_t& container_index = index.container_index;
    size_t& element_index = index.element_index;
    apply_to_elm(
      [&element_index, &value](auto&& container) {
        value = *std::next(std::begin(container), element_index);
      },
      containers, container_index);
  }
};

template <class... Types>
Factor(Types... containers)->Factor<Types...>;

} // namespace ParameterSweep
