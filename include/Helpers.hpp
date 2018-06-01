#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>
namespace ParameterSweep {
/*
 * A class that represents values at standard deviations above and below the
 * mean
 */
template <class NumericType>
class NormalFactor
{
public:
  NormalFactor(NumericType mean, NumericType stddev, size_t levels)
    : mean(mean)
    , stddev(stddev)
    , levels(levels)
  {}

  using value_type = NumericType;
  class iterator
  {
  public:
    using value_type = NumericType;
    using reference = value_type const&;
    using pointer = value_type const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator()
      : value()
      , factor(nullptr)
    {}
    iterator(NormalFactor* factor)
      : value(factor->mean - (factor->levels * factor->stddev))
      , factor(factor)
    {
      assert(factor->stddev > NumericType(0) &&
             "the standard deviation must be greater than 0");
    }
    bool operator==(iterator const& it) const
    {
      return (value == it.value && factor == it.factor) ||
             (is_endptr() && it.is_endptr());
    }
    bool operator!=(iterator const& it) const { return !(*this == it); }
    reference operator*() { return value; }
    reference operator->() { return value; }
    iterator& operator=(iterator const& it)
    {
      value = it.value;
      factor = it.factor;
      return *this;
    }
    iterator& operator++()
    {
      value += factor->stddev;
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

  private:
    bool is_endptr() const
    {
      return (factor == nullptr) || (value > factor->end_point());
    }
    NumericType value;
    NormalFactor* factor;
  };

  iterator begin() { return iterator(this); }
  iterator end() { return iterator(); }

private:
  inline NumericType end_point() const { return (levels * stddev) + mean; }
  NumericType mean, stddev;
  size_t levels;
};

/*
 * A class that represents values evenly spaced in a range
 */
template <class NumericType>
class RangeFactor
{
public:
  RangeFactor(NumericType min, NumericType max, size_t levels)
    : min(min)
    , max(max)
    , levels(levels)
  {}

  using value_type = NumericType;
  class iterator
  {
  public:
    using value_type = NumericType;
    using reference = value_type const&;
    using pointer = value_type const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator()
      : value()
      , factor(nullptr)
    {}
    iterator(RangeFactor* factor)
      : value(factor->min)
      , factor(factor)
    {
      assert(factor->min < factor->max &&
             "the max should be greater than the min");
      assert(factor->increment() > 0 &&
             "the iterator increment should be positive");
    }
    bool operator==(iterator const& it) const
    {
      return (value == it.value && factor == it.factor) ||
             (is_endptr() && it.is_endptr());
    }
    bool operator!=(iterator const& it) const { return !(*this == it); }
    reference operator*() { return value; }
    reference operator->() { return value; }
    iterator& operator=(iterator const& it)
    {
      value = it.value;
      factor = it.factor;
      return *this;
    }
    iterator& operator++()
    {
      value += factor->increment();
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

  private:
    bool is_endptr() const
    {
      return factor == nullptr || (value > factor->max);
    }
    NumericType value;
    RangeFactor* factor;
  };

  iterator begin() { return iterator(this); }
  iterator end() { return iterator(); }

private:
  NumericType increment() const { return (max - min + 1) / (levels); }
  NumericType min, max;
  size_t levels;
};

template <class NumericType, class RandomNumberGenerator>
class RandomFactor
{
public:
  RandomFactor(RandomNumberGenerator gen, size_t levels)
    : values(levels)
    , gen(gen)
  {
    std::generate(std::begin(values), std::end(values), gen);
  }
  using iterator = typename std::vector<NumericType>::const_iterator;
  using value_type = NumericType;

  auto begin() { return std::begin(values); }
  auto end() { return std::end(values); }
  size_t size() const { return std::size(values); }

private:
  std::vector<NumericType> values;
  RandomNumberGenerator gen;
};

template <class RandomNumberGenerator>
RandomFactor(RandomNumberGenerator gen, size_t levels)
  ->RandomFactor<decltype(std::declval<RandomNumberGenerator>()()),
                 RandomNumberGenerator>;

template <class Transform, class ForwardIt>
class TransformFactor
{
private:
  ForwardIt begin_it, end_it;
  Transform func;

public:
  TransformFactor(ForwardIt begin_it, ForwardIt end_it, Transform&& func)
    : begin_it(begin_it)
    , end_it(end_it)
    , func(std::forward<Transform>(func))
  {}
  using value_type = decltype(
    func(std::declval<typename std::iterator_traits<ForwardIt>::reference>()));
  class iterator
  {
  public:
    using value_type = value_type;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator(TransformFactor* factor)
      : factor(factor)
      , current(factor->begin_it)
      , value(factor->func(*current))
    {}
    iterator()
      : factor(nullptr)
      , current()
      , value()
    {}

    iterator& operator++()
    {
      current++;
      if (current != factor->end_it) {
        value = factor->func(*current);
      }
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    reference operator*() { return value; }
    reference operator->() { return value; }
    bool operator==(iterator const& it) const
    {
      return (is_endptr() && it.is_endptr()) ||
             (factor == it.factor && current == it.current);
    }
    bool operator!=(iterator const& it) { return !(*this == it); }

  private:
	bool is_endptr() const {
		return factor == nullptr || current == factor->end_it;
	}
    TransformFactor* factor;
    ForwardIt current;
    value_type value;
  };

  iterator begin() { return iterator(this); }
  iterator end() { return iterator(); }
};

} // namespace ParameterSweep
