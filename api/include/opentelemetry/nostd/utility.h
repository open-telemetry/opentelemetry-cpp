#pragma once

#include <cstddef>
#include <initializer_list>

namespace opentelemetry
{
namespace nostd
{
/**
 * Back port of std::data
 *
 * See https://en.cppreference.com/w/cpp/iterator/data
 */
template <class C>
auto data(C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class C>
auto data(const C &c) noexcept(noexcept(c.data())) -> decltype(c.data())
{
  return c.data();
}

template <class T, size_t N>
T *data(T (&array)[N]) noexcept
{
  return array;
}

template <class E>
const E *data(std::initializer_list<E> list) noexcept
{
  return list.begin();
}

/**
 * Back port of std::size
 *
 * See https://en.cppreference.com/w/cpp/iterator/size
 */
template <class C>
auto size(const C &c) noexcept(noexcept(c.size())) -> decltype(c.size())
{
  return c.size();
}

template <class T, size_t N>
size_t size(T (&array)[N]) noexcept
{
  return N;
}
}  // namespace nostd
}  // namespace opentelemetry
