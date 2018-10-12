#pragma once
#include <type_traits>

template <class T>
struct is_builder
{
private:
  template <class U>
  static auto test(int)
    -> decltype(std::declval<U>().get_parameters(0ull), std::true_type{});
  template <class>
  static std::false_type test(...);

public:
  static constexpr bool value =
    std::is_same_v<decltype(test<T>(0)), std::true_type>;
};
template <class T>
constexpr bool is_builder_v = is_builder<T>::value;

template <class T>
struct is_builder_it
{
private:
  template <class U>
  static auto test(int)
    -> decltype(std::declval<U>().get_parameters(), std::true_type{});
  template <class>
  static std::false_type test(...);

public:
  static constexpr bool value =
    std::is_same_v<decltype(test<T>(0)), std::true_type>;
};

template <class T>
constexpr bool is_builder_it_v = is_builder_it<T>::value;
