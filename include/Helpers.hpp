#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>
#include <cmath>
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
  {
      assert(stddev > NumericType(0) &&
             "the standard deviation must be greater than 0");
  }

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
    iterator(NormalFactor const* factor)
      : value(factor->mean - (factor->levels * factor->stddev))
      , factor(factor)
    {
    }
    bool operator==(iterator const& it) const
    {
		auto e1 = is_endptr();
		auto e2 = it.is_endptr();
      return (e1 && e2) || (!e1 && !e2 && value == it.value && factor == it.factor);
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
    NormalFactor const* factor;
  };

  iterator begin() const { return iterator(this); }
  iterator end() const { return iterator(); }
  size_t size() const { return levels*2 + 1; };

private:
  inline NumericType end_point() const { return (levels * stddev) + mean; }
  NumericType mean, stddev;
  size_t levels;
};

/*
 * A class that represents values evenly spaced in a range
 */
template<class T>
struct Arithmatic {
	static T increment(T current, T increment) {
		return current + increment;
	}
	static T step_size(T min, T max, size_t levels) {
		return  (max - min + 1)/levels;
	}

	static bool valid (T val) {
		return val > 0;
	}
};

template<class T>
struct Geometric {
	static T increment(T current, T increment) {
		return current * increment;
	}
	static T step_size(T min, T max, size_t levels) {
		double ret = exp(1.0/static_cast<double>(levels-1) * ( log(static_cast<double>(max)) - log(static_cast<double>(min))));
		if constexpr(std::is_integral<T>::value) {
			return std::round(ret);
		} else {
			return ret;
		}
	}
	static bool valid(T val) {
		return  val > 1.0;
	}
};


template <class NumericType, template <class> class Increment = Arithmatic>
class RangeFactor
{
public:
  using Step = Increment<NumericType>;

  RangeFactor(NumericType min, NumericType max, size_t levels)
    : min(min)
    , max(max)
	, step_size(Step::step_size(min,max,levels))
    , levels(levels)
  {
      assert(min < max &&
             "the max should be greater than the min");
      assert(Step::valid(step_size) &&
             "the iterator increment should be valid");
  }

  NumericType get_max() const {
	  return max;
  }
  NumericType get_min() const {
	  return min;
  }
  NumericType get_step_size() const {
	  return step_size();
  }

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
    iterator(RangeFactor const* factor)
      : value(factor->min)
      , factor(factor)
    {
    }
    bool operator==(iterator const& it) const
    {
	  auto e1 = is_endptr();
	  auto e2 = it.is_endptr();
      return (e1 && e2) || (!e1 && !e2 && value == it.value && factor == it.factor);
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
      value = Step::increment(value,factor->step_size);
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
    RangeFactor const* factor;
  };

  iterator begin() const { return iterator(this); }
  iterator end() const { return iterator(); }
  size_t size() const { return levels;}

private:
  NumericType min, max, step_size;
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

  auto begin() const { return std::begin(values); }
  auto end() const { return std::end(values); }
  size_t size() const { return std::size(values); }

private:
  std::vector<NumericType> values;
  RandomNumberGenerator gen;
};

template <class RandomNumberGenerator>
RandomFactor(RandomNumberGenerator gen, size_t levels)
  ->RandomFactor<decltype(std::declval<RandomNumberGenerator>()()),
                 RandomNumberGenerator>;

template <class Transform, class Container>
class TransformFactor
{
private:
  Container container;
  Transform func;

public:
  TransformFactor(Container container, Transform&& func)
    : container(container)
    , func(std::forward<Transform>(func))
  {}
  using value_type = decltype(
    func(std::declval<typename std::iterator_traits<typename Container::iterator>::reference>()));
  class iterator
  {
  public:
    using value_type = value_type;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = typename std::iterator_traits<typename Container::iterator>::iterator_category;

    iterator(TransformFactor const* factor)
      : factor(factor)
      , current(std::begin(factor->container))
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
      if (current != std::end(factor->container)) {
        value = factor->func(*current);
      }
      return *this;
    }
    iterator& operator--()
    {
      current--;
      if (current != std::end(factor->container)) {
        value = factor->func(*current);
      }
      return *this;
    }
    iterator operator--(int)
    {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }
    iterator operator++(int)
    {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

	iterator operator+(difference_type n) {
		iterator tmp = *this;
		tmp+=n;
		return tmp;
	}
	iterator& operator+=(difference_type n) {
		current += n;
		if(current != std::end(factor->container)) {
			value = factor->func(*current);
		}
		return *this;
	}
	iterator& operator-=(difference_type n) {*this += (-n); return *this;}
	iterator operator-(difference_type n) { return *this + (-n); }
	difference_type operator-(iterator const& it) {
		return current - it.current;
	}
	value_type operator[](size_t n) const {
		return factor->func(*(current+n));
	}

    reference operator*() { return value; }
    reference operator->() { return value; }
    bool operator==(iterator const& it) const
    {
		auto e1 = is_endptr();
		auto e2 = it.is_endptr();
      return (e1 && e2) ||
             (!e1 && !e2 && factor == it.factor && current == it.current);
    }
    bool operator!=(iterator const& it) const { return !(*this == it); }
	bool operator<(iterator const& it) const { return current < it.current;}
	bool operator>(iterator const& it) const { return it.current < current;}
	bool operator<=(iterator const& it) const {return !(*this < it);}
	bool operator>=(iterator const& it) const {return !( it< *this );}

  private:
	bool is_endptr() const {
		return factor == nullptr || current == std::end(factor->container);
	}
    TransformFactor const* factor;
	typename Container::iterator current;
    value_type value;
  };

  iterator begin() const { return iterator(this); }
  iterator end() const { return iterator(); }
  size_t size() const { return std::size(container); }
};

} // namespace ParameterSweep
