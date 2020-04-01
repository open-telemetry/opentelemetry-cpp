#pragma once

#include <array>
#include <exception>

#include "opentelemetry/nostd/detail/common_trait.h"
#include "opentelemetry/nostd/detail/find_index.h"
#include "opentelemetry/nostd/detail/type_pack_element.h"
#include "opentelemetry/nostd/detail/variant_alternative.h"
#include "opentelemetry/nostd/detail/variant_fwd.h"
#include "opentelemetry/nostd/detail/variant_size.h"
#include "opentelemetry/nostd/utility.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{

#define AUTO auto
#define AUTO_RETURN(...) \
  -> decay_t<decltype(__VA_ARGS__)> { return __VA_ARGS__; }

#define AUTO_REFREF auto
#define AUTO_REFREF_RETURN(...)                                           \
  ->decltype((__VA_ARGS__))                                               \
  {                                                                       \
    static_assert(std::is_reference<decltype((__VA_ARGS__))>::value, ""); \
    return __VA_ARGS__;                                                   \
  }

#define DECLTYPE_AUTO auto
#define DECLTYPE_AUTO_RETURN(...) \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

class bad_variant_access : public std::exception
{
public:
  virtual const char *what() const noexcept override { return "bad_variant_access"; }
};

[[noreturn]] inline void throw_bad_variant_access()
{
#if __EXCEPTIONS
  throw bad_variant_access{};
#else
  std::terminate();
#endif
}

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
    inline constexpr AUTO_REFREF operator()(V &&v) const
        AUTO_REFREF_RETURN(get_alt_impl<I - 1>{}(std::forward<V>(v).tail_))
  };

  template <bool Dummy>
  struct get_alt_impl<0, Dummy>
  {
    template <typename V>
    inline constexpr AUTO_REFREF operator()(V &&v) const
        AUTO_REFREF_RETURN(std::forward<V>(v).head_)
  };

  template <typename V, std::size_t I>
  inline static constexpr AUTO_REFREF get_alt(V &&v, in_place_index_t<I>)
      AUTO_REFREF_RETURN(get_alt_impl<I>{}(std::forward<V>(v)))
};

struct base
{
  template <std::size_t I, typename V>
  inline static constexpr AUTO_REFREF get_alt(V &&v)
#ifdef _MSC_VER
      AUTO_REFREF_RETURN(recursive_union::get_alt(std::forward<V>(v).data_, in_place_index_t<I>{}))
#else
      AUTO_REFREF_RETURN(recursive_union::get_alt(data(std::forward<V>(v)), in_place_index_t<I>{}))
#endif
};

struct variant
{
  template <std::size_t I, typename V>
  inline static constexpr AUTO_REFREF get_alt(V &&v)
      AUTO_REFREF_RETURN(base::get_alt<I>(std::forward<V>(v).impl_))
};
}  // namespace access
}  // namespace detail

namespace detail
{
namespace visitation
{

struct base
{
  template <typename Visitor, typename... Vs>
  using dispatch_result_t = decltype(
      invoke(std::declval<Visitor>(), access::base::get_alt<0>(std::declval<Vs>())...));

  template <typename Expected>
  struct expected
  {
    template <typename Actual>
    inline static constexpr bool but_got()
    {
      return std::is_same<Expected, Actual>::value;
    }
  };

  template <typename Expected, typename Actual>
  struct visit_return_type_check
  {
    static_assert(expected<Expected>::template but_got<Actual>(),
                  "`visit` requires the visitor to have a single return type");

    template <typename Visitor, typename... Alts>
    inline static constexpr DECLTYPE_AUTO invoke(Visitor &&visitor, Alts &&... alts)
        DECLTYPE_AUTO_RETURN(invoke(std::forward<Visitor>(visitor), std::forward<Alts>(alts)...))
  };

  template <typename T>
  inline static constexpr const T &at(const T &elem) noexcept
  {
    return elem;
  }

#if 0
  template <typename T, std::size_t N, typename... Is>
  inline static constexpr const lib::remove_all_extents_t<T> &at(const std::array<T, N> &elems,
                                                                 std::size_t i,
                                                                 Is... is) noexcept
  {
    return at(elems[i], is...);
  }

  template <typename F, typename... Fs>
  inline static constexpr lib::array<lib::decay_t<F>, sizeof...(Fs) + 1> make_farray(F &&f,
                                                                                     Fs &&... fs)
  {
    return {{std::forward<F>(f), std::forward<Fs>(fs)...}};
  }

  template <typename F, typename... Vs>
  struct make_fmatrix_impl
  {

    template <std::size_t... Is>
    inline static constexpr dispatch_result_t<F, Vs...> dispatch(F &&f, Vs &&... vs)
    {
      using Expected = dispatch_result_t<F, Vs...>;
      using Actual   = decltype(
          lib::invoke(std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...);
    }

    template <typename...>
    struct impl;

    template <std::size_t... Is>
    struct impl<lib::index_sequence<Is...>>
    {
      inline constexpr AUTO operator()() const AUTO_RETURN(&dispatch<Is...>)
    };

    template <typename Is, std::size_t... Js, typename... Ls>
    struct impl<Is, lib::index_sequence<Js...>, Ls...>
    {
      inline constexpr AUTO operator()() const
          AUTO_RETURN(make_farray(impl<lib::push_back_t<Is, Js>, Ls...>{}()...))
    };
  };

  template <typename F, typename... Vs>
  inline static constexpr AUTO make_fmatrix()
      AUTO_RETURN(typename make_fmatrix_impl<F, Vs...>::template impl<
                  lib::index_sequence<>,
                  lib::make_index_sequence<lib::decay_t<Vs>::size()>...>{}())

          template <typename F, typename... Vs>
          struct make_fdiagonal_impl
  {
    template <std::size_t I>
    inline static constexpr dispatch_result_t<F, Vs...> dispatch(F &&f, Vs &&... vs)
    {
      using Expected = dispatch_result_t<F, Vs...>;
      using Actual   = decltype(
          lib::invoke(std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...);
    }

    template <std::size_t... Is>
    inline static constexpr AUTO impl(lib::index_sequence<Is...>)
        AUTO_RETURN(make_farray(&dispatch<Is>...))
  };

  template <typename F, typename V, typename... Vs>
  inline static constexpr auto make_fdiagonal() -> decltype(
      make_fdiagonal_impl<F, V, Vs...>::impl(lib::make_index_sequence<lib::decay_t<V>::size()>{}))
  {
    static_assert(lib::all<(lib::decay_t<V>::size() == lib::decay_t<Vs>::size())...>::value,
                  "all of the variants must be the same size.");
    return make_fdiagonal_impl<F, V, Vs...>::impl(
        lib::make_index_sequence<lib::decay_t<V>::size()>{});
  }
};

#if !defined(MPARK_VARIANT_SWITCH_VISIT) && (!defined(_MSC_VER) || _MSC_VER >= 1910)
template <typename F, typename... Vs>
using fmatrix_t = decltype(base::make_fmatrix<F, Vs...>());

template <typename F, typename... Vs>
struct fmatrix
{
  static constexpr fmatrix_t<F, Vs...> value = base::make_fmatrix<F, Vs...>();
};

template <typename F, typename... Vs>
constexpr fmatrix_t<F, Vs...> fmatrix<F, Vs...>::value;

template <typename F, typename... Vs>
using fdiagonal_t = decltype(base::make_fdiagonal<F, Vs...>());

template <typename F, typename... Vs>
struct fdiagonal
{
  static constexpr fdiagonal_t<F, Vs...> value = base::make_fdiagonal<F, Vs...>();
};

template <typename F, typename... Vs>
constexpr fdiagonal_t<F, Vs...> fdiagonal<F, Vs...>::value;
#endif

struct alt
{
  template <typename Visitor, typename... Vs>
  inline static constexpr DECLTYPE_AUTO visit_alt(Visitor &&visitor, Vs &&... vs)
      DECLTYPE_AUTO_RETURN(
          base::at(base::make_fmatrix<Visitor &&, decltype(as_base(std::forward<Vs>(vs)))...>(),
                   vs.index()...)(std::forward<Visitor>(visitor), as_base(std::forward<Vs>(vs))...))

          template <typename Visitor, typename... Vs>
          inline static constexpr DECLTYPE_AUTO
      visit_alt_at(std::size_t index, Visitor &&visitor, Vs &&... vs) DECLTYPE_AUTO_RETURN(
          base::at(base::make_fdiagonal<Visitor &&, decltype(as_base(std::forward<Vs>(vs)))...>(),
                   index)(std::forward<Visitor>(visitor), as_base(std::forward<Vs>(vs))...))
};

struct variant
{
private:
  template <typename Visitor>
  struct visitor
  {
    template <typename... Values>
    inline static constexpr bool does_not_handle()
    {
      return lib::is_invocable<Visitor, Values...>::value;
    }
  };

  template <typename Visitor, typename... Values>
  struct visit_exhaustiveness_check
  {
    static_assert(visitor<Visitor>::template does_not_handle<Values...>(),
                  "`visit` requires the visitor to be exhaustive.");

    inline static constexpr DECLTYPE_AUTO invoke(Visitor &&visitor, Values &&... values)
        DECLTYPE_AUTO_RETURN(lib::invoke(std::forward<Visitor>(visitor),
                                         std::forward<Values>(values)...))
  };

  template <typename Visitor>
  struct value_visitor
  {
    Visitor &&visitor_;

    template <typename... Alts>
    inline constexpr DECLTYPE_AUTO operator()(Alts &&... alts) const DECLTYPE_AUTO_RETURN(
        visit_exhaustiveness_check<Visitor, decltype((std::forward<Alts>(alts).value))...>::invoke(
            std::forward<Visitor>(visitor_),
            std::forward<Alts>(alts).value...))
  };

  template <typename Visitor>
  inline static constexpr AUTO make_value_visitor(Visitor &&visitor)
      AUTO_RETURN(value_visitor<Visitor>{std::forward<Visitor>(visitor)})

          public
      : template <typename Visitor, typename... Vs>
        inline static constexpr DECLTYPE_AUTO visit_alt(Visitor &&visitor, Vs &&... vs)
            DECLTYPE_AUTO_RETURN(alt::visit_alt(std::forward<Visitor>(visitor),
                                                std::forward<Vs>(vs).impl_...))

                template <typename Visitor, typename... Vs>
                inline static constexpr DECLTYPE_AUTO
        visit_alt_at(std::size_t index, Visitor &&visitor, Vs &&... vs) DECLTYPE_AUTO_RETURN(
            alt::visit_alt_at(index, std::forward<Visitor>(visitor), std::forward<Vs>(vs).impl_...))

            template <typename Visitor, typename... Vs>
            inline static constexpr DECLTYPE_AUTO visit_value(Visitor &&visitor, Vs &&... vs)
                DECLTYPE_AUTO_RETURN(visit_alt(make_value_visitor(std::forward<Visitor>(visitor)),
                                               std::forward<Vs>(vs)...))

                    template <typename Visitor, typename... Vs>
                    inline static constexpr DECLTYPE_AUTO
        visit_value_at(std::size_t index, Visitor &&visitor, Vs &&... vs)
            DECLTYPE_AUTO_RETURN(visit_alt_at(index,
                                              make_value_visitor(std::forward<Visitor>(visitor)),
                                              std::forward<Vs>(vs)...))
#endif
};
}  // namespace visitation
}  // namespace detail

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
