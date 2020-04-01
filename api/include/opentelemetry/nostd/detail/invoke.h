#pragma once

#include <type_traits>
#include <utility>

#include "opentelemetry/nostd/detail/decay.h"
#include "opentelemetry/version.h"

#define MPARK_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
namespace detail
{

template <typename T>
struct is_reference_wrapper : std::false_type
{};

template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
{};

template <bool, int>
struct Invoke;

template <>
struct Invoke<true /* pmf */, 0 /* is_base_of */>
{
  template <typename R, typename T, typename Arg, typename... Args>
  inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
      MPARK_RETURN((std::forward<Arg>(arg).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 1 /* is_reference_wrapper */>
{
  template <typename R, typename T, typename Arg, typename... Args>
  inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
      MPARK_RETURN((std::forward<Arg>(arg).get().*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 2 /* otherwise */>
{
  template <typename R, typename T, typename Arg, typename... Args>
  inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
      MPARK_RETURN(((*std::forward<Arg>(arg)).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<false /* pmo */, 0 /* is_base_of */>
{
  template <typename R, typename T, typename Arg>
  inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
      MPARK_RETURN(std::forward<Arg>(arg).*pmo)
};

template <>
struct Invoke<false /* pmo */, 1 /* is_reference_wrapper */>
{
  template <typename R, typename T, typename Arg>
  inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
      MPARK_RETURN(std::forward<Arg>(arg).get().*pmo)
};

template <>
struct Invoke<false /* pmo */, 2 /* otherwise */>
{
  template <typename R, typename T, typename Arg>
  inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
      MPARK_RETURN((*std::forward<Arg>(arg)).*pmo)
};

template <typename R, typename T, typename Arg, typename... Args>
inline constexpr auto invoke(R T::*f, Arg &&arg, Args &&... args)
    MPARK_RETURN(Invoke<std::is_function<R>::value,
                        (std::is_base_of<T, decay_t<Arg>>::value
                             ? 0
                             : is_reference_wrapper<decay_t<Arg>>::value ? 1 : 2)>::
                     invoke(f, std::forward<Arg>(arg), std::forward<Args>(args)...))

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4100)
#endif
        template <typename F, typename... Args>
        inline constexpr auto invoke(F &&f, Args &&... args)
            MPARK_RETURN(std::forward<F>(f)(std::forward<Args>(args)...))
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
}  // namespace detail

template <typename F, typename... Args>
inline constexpr auto invoke(F &&f, Args &&... args)
    MPARK_RETURN(detail::invoke(std::forward<F>(f), std::forward<Args>(args)...))
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

#undef MPARK_RETURN
