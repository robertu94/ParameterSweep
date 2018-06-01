#pragma once
#include <tuple>
#include <type_traits>

constexpr bool Writable = true;
constexpr bool ReadOnly = false;

template <class T, class ValueType, class IteratorTag, bool IsWritable,
          class DifferenceType>
struct IteratorTest
{
  using Reference =
    std::conditional_t<IsWritable == Writable, ValueType&, ValueType const&>;
  using Pointer = std::conditional_t<IsWritable, ValueType*, ValueType const*>;

  static_assert(
    std::is_same<typename T::iterator::value_type, ValueType>::value,
    "iterator value type incorrect");

  static_assert(std::is_same<typename T::iterator::reference, Reference>::value,
                "iterator reference type incorrect");

  static_assert(std::is_same<typename T::iterator::pointer, Pointer>::value,
                "iterator pointer type incorrect");
  static_assert(
    std::is_same<typename T::iterator::difference_type, DifferenceType>::value,
    "iterator difference_type type incorrect");

  static_assert(std::is_base_of<IteratorTag,
                                typename T::iterator::iterator_category>::value,
                "iterator category type incorrect");

  static_assert(std::is_copy_constructible<T>::value, "is not copy constructible");
  static_assert(std::is_destructible<T>::value, "is not destructable");
};

template <class ValueType, class IteratorTag, bool Writable,
          class DifferenceType = std::ptrdiff_t, class... Ts>
struct TestAllIterators
{
  std::tuple<
    IteratorTest<Ts, ValueType, IteratorTag, Writable, DifferenceType>...>
    tests;
};

template <class ValueType, class... Ts>
using TestForwardWritable =
  TestAllIterators<ValueType, std::forward_iterator_tag, Writable,
                   std::ptrdiff_t>;

template <class ValueType, class... Ts>
using TestForwardReadOnly =
  TestAllIterators<ValueType, std::forward_iterator_tag, ReadOnly,
                   std::ptrdiff_t>;
