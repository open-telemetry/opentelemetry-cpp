#pragma once

#include <cstddef>
#include <iterator>
#include <cassert>
#include <array>
#include <type_traits>

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
 */
template <class T, size_t Extent = dynamic_extent>
class span {
 public:
   static constexpr size_t extent = Extent;

   template <typename std::enable_if<Extent == 0> * = nullptr>
   span() noexcept : data_{nullptr}
   {}

   span(T* data, size_t count) noexcept : data_{data} {
     assert(count == Extent);
   }

   span(T *first, T *last) noexcept
       : data_{first}
   {
     assert(std::distance(first, last) == Extent);
   }

   template <size_t N>
   span(T (&array)[N]) noexcept : data_{array}
   {
     assert(N == Extent);
   }

   template <size_t N>
   span(std::array<T, N>& array) noexcept : data_{array.data()} {
     assert(N == Extent);
   }

   template <size_t N>
   span(const std::array<T, N>& array) noexcept : data_{array.data()} {
     assert(N == Extent);
   }

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
   span(T (&arr)[N]) noexcept : extent_{N}, data_{arr}
   {}

   template <size_t N>
   span(std::array<T, N>& array) noexcept : extent_{N}, data_{array.data()} {
   }

   template <size_t N>
   span(const std::array<T, N> &array) noexcept : extent_{N}, data_{array.data()}
   {
   }

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
