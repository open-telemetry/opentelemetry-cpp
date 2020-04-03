#pragma once

#include <utility>
#include <type_traits>

#include "opentelemetry/nostd/detail/trait.h"
#include "opentelemetry/nostd/detail/valueless.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

#define AUTO_REFREF_RETURN(...)                                           \
  ->decltype((__VA_ARGS__))                                               \
  {                                                                       \
    static_assert(std::is_reference<decltype((__VA_ARGS__))>::value, ""); \
    return __VA_ARGS__;                                                   \
  }

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
namespace detail
{
namespace access
{
struct recursive_union
{
  template <std::size_t I, bool Dummy = true>
  struct get_alt_impl
  {
    template <typename V>
    inline constexpr auto operator()(V &&v) const
        AUTO_REFREF_RETURN(get_alt_impl<I - 1>{}(std::forward<V>(v).tail_))
  };

  template <bool Dummy>
  struct get_alt_impl<0, Dummy>
  {
    template <typename V>
    inline constexpr auto operator()(V &&v) const
        AUTO_REFREF_RETURN(std::forward<V>(v).head_)
  };

  template <typename V, std::size_t I>
  inline static constexpr auto get_alt(V &&v, in_place_index_t<I>)
      AUTO_REFREF_RETURN(get_alt_impl<I>{}(std::forward<V>(v)))
};
} // namespace access

template <std::size_t Index, typename T>
struct alt
{
  using value_type = T;

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)
#endif
  template <typename... Args>
  inline explicit constexpr alt(in_place_t, Args &&... args) : value(std::forward<Args>(args)...)
  {}
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

  T value;
};

template <Trait DestructibleTrait, std::size_t Index, typename... Ts>
union recursive_union;

template <Trait DestructibleTrait, std::size_t Index>
union recursive_union<DestructibleTrait, Index>
{};

#define OPENTELEMETRY_VARIANT_RECURSIVE_UNION(destructible_trait, destructor)               \
  template <std::size_t Index, typename T, typename... Ts>                          \
  union recursive_union<destructible_trait, Index, T, Ts...>                        \
  {                                                                                 \
  public:                                                                           \
    inline explicit constexpr recursive_union(valueless_t) noexcept : dummy_{} {}   \
                                                                                    \
    template <typename... Args>                                                     \
    inline explicit constexpr recursive_union(in_place_index_t<0>, Args &&... args) \
        : head_(in_place_t{}, std::forward<Args>(args)...)                          \
    {}                                                                              \
                                                                                    \
    template <std::size_t I, typename... Args>                                      \
    inline explicit constexpr recursive_union(in_place_index_t<I>, Args &&... args) \
        : tail_(in_place_index_t<I - 1>{}, std::forward<Args>(args)...)             \
    {}                                                                              \
                                                                                    \
    recursive_union(const recursive_union &) = default;                             \
    recursive_union(recursive_union &&)      = default;                             \
                                                                                    \
    destructor                                                                      \
                                                                                    \
        recursive_union &                                                           \
        operator=(const recursive_union &) = default;                               \
    recursive_union &operator=(recursive_union &&) = default;                       \
                                                                                    \
  private:                                                                          \
    char dummy_;                                                                    \
    alt<Index, T> head_;                                                            \
    recursive_union<destructible_trait, Index + 1, Ts...> tail_;                    \
                                                                                    \
    friend struct access::recursive_union;                                          \
  }

OPENTELEMETRY_VARIANT_RECURSIVE_UNION(Trait::TriviallyAvailable, ~recursive_union() = default;);
OPENTELEMETRY_VARIANT_RECURSIVE_UNION(Trait::Available, ~recursive_union(){});
OPENTELEMETRY_VARIANT_RECURSIVE_UNION(Trait::Unavailable, ~recursive_union() = delete;);

#undef OPENTELEMETRY_VARIANT_RECURSIVE_UNION
}  // namespace detail
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

#undef AUTO_REFREF_RETURN
