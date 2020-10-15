#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/span_context_kv_iterable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace detail
{
template <class T>
inline void take_span_context_kv(SpanContext, opentelemetry::trace::KeyValueIterableView<T>)
{}

template <class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
inline void take_span_context_kv(SpanContext, T &)
{}

inline void take_span_context_kv(
    SpanContext,
    std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>)
{}

template <class T>
auto is_span_context_kv_iterable_impl(T iterable)
    -> decltype(take_span_context_kv(std::begin(iterable)->first, std::begin(iterable)->second),
                nostd::size(iterable),
                std::true_type{});

std::false_type is_span_context_kv_iterable_impl(...);

template <class T>
struct is_span_context_kv_iterable
{
  static const bool value =
      decltype(detail::is_span_context_kv_iterable_impl(std::declval<T>()))::value;
};
}  // namespace detail

template <class T>
class SpanContextKeyValueIterableView final : public SpanContextKeyValueIterable
{
  static_assert(detail::is_span_context_kv_iterable<T>::value,
                "Must be a context/key-value iterable");

public:
  explicit SpanContextKeyValueIterableView(const T &links) noexcept : container_{&links} {}

  bool ForEachKeyValue(
      nostd::function_ref<bool(SpanContext, nostd::string_view, common::AttributeValue, bool)>
          callback) const noexcept override
  {
    auto iter = std::begin(*container_);
    auto last = std::end(*container_);
    for (; iter != last; ++iter)
    {
      auto kv_iter = std::begin(iter->second);
      auto kv_end  = std::end(iter->second);
      // attributes are optional, and so may be empty container
      if (kv_iter == kv_end)
      {
        if (!callback(iter->first, "", static_cast<nostd::string_view>(""), true))
        {
          return false;
        }
      }
      else
      {
        auto kv_last    = std::prev(kv_end);
        bool is_last_kv = false;
        for (; kv_iter != kv_end; ++kv_iter)
        {
          if (kv_iter == kv_last)
          {
            is_last_kv = true;
          }
          if (!callback(iter->first, kv_iter->first, kv_iter->second, is_last_kv))
          {
            return false;
          }
        }
      }
    }
    return true;
  }

  size_t size() const noexcept override { return nostd::size(*container_); }

private:
  const T *container_;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
