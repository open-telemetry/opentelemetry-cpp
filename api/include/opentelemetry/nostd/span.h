#pragma once

#include <exception>
#include <cstddef>
#include <iterator>
#include <cassert>
#include <array>
#include <type_traits>

#include "opentelemetry/nostd/utility.h"

namespace opentelemetry
{
namespace nostd
{
constexpr size_t dynamic_extent = static_cast<size_t>(-1);

/**
 * Back port of std::span.
 *
 * See https://en.cppreference.com/w/cpp/container/span for interface documentation.
 *
 * Note: This provides a subset of the methods available on std::span.
 *
 * Note: The std::span API specifies error cases to have undefined behavior, so this implementation
 * chooses to terminate or assert rather than throw exceptions.
 */
template <class T, size_t Extent = dynamic_extent>
class span {
 public:
   static constexpr size_t extent = Extent;

   // This arcane code is how we make default-construction result in an SFINAE error
   // with C++11 when Extent != 0 as specified by the std::span API.
   //
   // See https://stackoverflow.com/a/10309720/4447365
   template <bool B = Extent == 0, typename std::enable_if<B>::type * = nullptr>
   span() noexcept : data_{nullptr}
   {}

   span(T* data, size_t count) noexcept : data_{data} {
     if (count != Extent) {
       std::terminate();
     }
   }

   span(T *first, T *last) noexcept : data_{first}
   {
     if (std::distance(first, last) != Extent)
     {
       std::terminate();
     }
   }

   template <size_t N, typename std::enable_if<Extent == N>::type* = nullptr>
   span(T (&array)[N]) noexcept : data_{array}
   {
   }

   template <size_t N, typename std::enable_if<Extent == N>::type* = nullptr>
   span(std::array<T, N>& array) noexcept : data_{array.data()} {
   }

   template <size_t N, typename std::enable_if<Extent == N>::type* = nullptr>
   span(const std::array<T, N>& array) noexcept : data_{array.data()} {
   }

   template <class U,
             size_t N,
             typename std::enable_if<
                 N == Extent && std::is_convertible<U (*)[], T (*)[]>::value>::type * = nullptr>
   span(const span<U, N> &other) noexcept : data_{other.data()}
   {}

   span(const span&) noexcept = default;

   bool empty() const noexcept { return Extent == 0; }

   T *data() const noexcept { return data_; }

   size_t size() const noexcept { return Extent; }

   T &operator[](size_t index) const noexcept
   {
     assert(index < Extent);
     return data_[index];
    }

    T* begin() const noexcept { return data_; }

    T* end() const noexcept { return data_ + Extent; }
 private:
   T* data_;
};

template <class T>
class span<T, dynamic_extent> {
 public:
   static constexpr size_t extent = dynamic_extent;
   
   span() noexcept : extent_{0}, data_{nullptr} {}

   span(T* data, size_t count) noexcept : extent_{count}, data_{data} {}

   span(T *first, T *last) noexcept
       : extent_{static_cast<size_t>(std::distance(first, last))}, data_{first}
   {
     assert(first <= last);
   }

   template <size_t N>
   span(T (&array)[N]) noexcept : extent_{N}, data_{array}
   {}

   template <size_t N>
   span(std::array<T, N>& array) noexcept : extent_{N}, data_{array.data()} {
   }

   template <size_t N>
   span(const std::array<T, N>& array) noexcept : extent_{N}, data_{array.data()} {
   }

   template <
       class U,
       size_t N,
       typename std::enable_if<std::is_convertible<U (*)[], T (*)[]>::value>::type * = nullptr>
   span(const span<U, N> &other) noexcept : extent_{other.size()}, data_{other.data()}
   {}

   span(const span&) noexcept = default;

   bool empty() const noexcept { return extent_ == 0; }

   T *data() const noexcept { return data_; }

   size_t size() const noexcept { return extent_; }

   T &operator[](size_t index) const noexcept
   {
     assert(index < extent_);
     return data_[index];
    }

    T* begin() const noexcept { return data_; }

    T* end() const noexcept { return data_ + extent_; }
 private:
   // Note: matches libstdc++'s layout for std::span
   // See
   // https://github.com/gcc-mirror/gcc/blob/a60701e05b3878000ff9fdde1aecbc472b9dec5a/libstdc%2B%2B-v3/include/std/span#L402-L403
   size_t extent_;
   T* data_;
};
}  // namespace nostd
}  // namespace opentelemetry
