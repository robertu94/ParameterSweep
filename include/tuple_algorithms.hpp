#pragma once
#include <algorithm>
#include <tuple>
#include <array>

namespace {
template <class Func, class Tuple, std::size_t... Is>
void tuple_for_each_impl(Func func, Tuple const &t,
                         std::index_sequence<Is...>) {
  (func(std::get<Is>(t)), ...);
}

template <class Func, class Tuple, std::size_t... Is>
auto tuple_transform_impl(Func func, Tuple const &t,
                          std::index_sequence<Is...>) {
  return std::make_tuple(func(std::get<Is>(t))...);
}

template <class Func, class TupleT, class TupleU, std::size_t... Is>
auto tuple_transform_impl(Func func, TupleT const &t, TupleU const& u, std::index_sequence<Is...>) {
  return std::make_tuple(func((std::get<Is>(t)),std::get<Is>(u))...);
}

template <class Type, class Tuple, std::size_t... Is>
auto tuple_to_array_impl(Tuple const &t, std::index_sequence<Is...>) {
	return std::array<Type,std::tuple_size<Tuple>::value>{ std::get<Is>(t) ... };
}

template <class Array, std::size_t... Is>
auto array_to_tuple_impl(Array const& array, std::index_sequence<Is...>)
{
	return std::make_tuple(array[Is]...);
}

template<class Type, class Func>
void do_apply(size_t is, size_t idx, Func&& func, Type&& tuple_element)
{
	if (is==idx)
		std::forward<Func>(func)(std::forward<Type>(tuple_element));
}

template <class Func, class Tuple, std::size_t ...Is>
void apply_to_elm_impl (Func&& func, Tuple&& t, std::size_t index, std::index_sequence<Is...>)
{
	(do_apply(Is,index,std::forward<Func>(func),std::get<Is>(t)),...);
}

} // namespace

template <class Func, class... T>
void tuple_for_each(Func func, std::tuple<T...> const &t) {
  tuple_for_each_impl(func, t, std::index_sequence_for<T...>{});
}

template <class Func, class... T>
auto tuple_transform(Func func, std::tuple<T...> const &t) {
  return tuple_transform_impl(func, t, std::index_sequence_for<T...>{});
}

template <class Func, class... T, class... U>
auto tuple_transform(Func func, std::tuple<T...> const& t, std::tuple<U...> const& u) {
  return tuple_transform_impl(func, t, u, std::index_sequence_for<T...>{});
}

template <class Type, class... T>
auto tuple_to_array(std::tuple<T...> const& t)
{
	return tuple_to_array_impl<Type>(t, std::index_sequence_for<T...>{});
}

template <class T, size_t N>
auto array_to_tuple(std::array<T,N> const& a)
{
	return array_to_tuple_impl(a, std::make_index_sequence<N>{});
}

template <class Func,  class...Types>
void apply_to_elm(Func func, std::tuple<Types...> const& t, size_t index)
{
	apply_to_elm_impl(func, t, index, std::index_sequence_for<Types...>{});
}

template <class Type, class... Types>
void get_runtime(std::tuple<Types...> const& t, size_t index, Type& elm)
{
	apply_to_elm_impl([&elm](auto&& i){elm = i;}, t, index, std::index_sequence_for<Types...>{});
}
