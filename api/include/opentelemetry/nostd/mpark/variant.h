// MPark.Variant
//
// Copyright Michael Park, 2015-2017
// Copyright OpenTelemetry Authors, 2020
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file third_party/boost/LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#pragma once

#include <array>
#include <exception>
#include <limits>

#include "opentelemetry/nostd/detail/all.h"
#include "opentelemetry/nostd/detail/dependent_type.h"
#include "opentelemetry/nostd/detail/find_index.h"
#include "opentelemetry/nostd/detail/functional.h"
#include "opentelemetry/nostd/detail/recursive_union.h"
#include "opentelemetry/nostd/detail/trait.h"
#include "opentelemetry/nostd/detail/type_pack_element.h"
#include "opentelemetry/nostd/detail/variant_alternative.h"
#include "opentelemetry/nostd/detail/variant_fwd.h"
#include "opentelemetry/nostd/detail/variant_size.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

#define AUTO_RETURN(...) \
  ->decay_t<decltype(__VA_ARGS__)> { return __VA_ARGS__; }

#define AUTO_REFREF_RETURN(...)                                           \
  ->decltype((__VA_ARGS__))                                               \
  {                                                                       \
    static_assert(std::is_reference<decltype((__VA_ARGS__))>::value, ""); \
    return __VA_ARGS__;                                                   \
  }

#define DECLTYPE_AUTO_RETURN(...) \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
constexpr std::size_t variant_npos = static_cast<std::size_t>(-1);

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
struct base
{
  template <std::size_t I, typename V>
  inline static constexpr auto get_alt(V &&v)
#ifdef _MSC_VER
      AUTO_REFREF_RETURN(recursive_union::get_alt(std::forward<V>(v).data_, in_place_index_t<I>{}))
#else
      AUTO_REFREF_RETURN(recursive_union::get_alt(data(std::forward<V>(v)), in_place_index_t<I>{}))
#endif
};

struct variant
{
  template <std::size_t I, typename V>
  inline static constexpr auto get_alt(V &&v)
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
      nostd::invoke(std::declval<Visitor>(), access::base::get_alt<0>(std::declval<Vs>())...));

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
    inline static constexpr auto invoke(Visitor &&visitor, Alts &&... alts)
        DECLTYPE_AUTO_RETURN(nostd::invoke(std::forward<Visitor>(visitor),
                                           std::forward<Alts>(alts)...))
  };

  template <typename T>
  inline static constexpr const T &at(const T &elem) noexcept
  {
    return elem;
  }

  template <typename T, std::size_t N, typename... Is>
  inline static constexpr const remove_all_extents_t<T> &at(const std::array<T, N> &elems,
                                                            std::size_t i,
                                                            Is... is) noexcept
  {
    return at(elems[i], is...);
  }

  template <typename F, typename... Fs>
  inline static constexpr std::array<decay_t<F>, sizeof...(Fs) + 1> make_farray(F &&f, Fs &&... fs)
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
          nostd::invoke(std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...);
    }

    template <typename...>
    struct impl;

    template <std::size_t... Is>
    struct impl<index_sequence<Is...>>
    {
      inline constexpr auto operator()() const AUTO_RETURN(&dispatch<Is...>)
    };

    template <typename Is, std::size_t... Js, typename... Ls>
    struct impl<Is, index_sequence<Js...>, Ls...>
    {
      inline constexpr auto operator()() const
          AUTO_RETURN(make_farray(impl<detail::index_sequence_push_back_t<Is, Js>, Ls...>{}()...))
    };
  };

  template <typename F, typename... Vs>
  inline static constexpr auto make_fmatrix() AUTO_RETURN(
      typename make_fmatrix_impl<F, Vs...>::
          template impl<index_sequence<>, make_index_sequence<decay_t<Vs>::size()>...>{}())

      template <typename F, typename... Vs>
      struct make_fdiagonal_impl
  {
    template <std::size_t I>
    inline static constexpr dispatch_result_t<F, Vs...> dispatch(F &&f, Vs &&... vs)
    {
      using Expected = dispatch_result_t<F, Vs...>;
      using Actual   = decltype(
          nostd::invoke(std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...);
    }

    template <std::size_t... Is>
    inline static constexpr auto impl(index_sequence<Is...>)
        AUTO_RETURN(make_farray(&dispatch<Is>...))
  };

  template <typename F, typename V, typename... Vs>
  inline static constexpr auto make_fdiagonal()
      -> decltype(make_fdiagonal_impl<F, V, Vs...>::impl(make_index_sequence<decay_t<V>::size()>{}))
  {
    static_assert(all<(decay_t<V>::size() == decay_t<Vs>::size())...>::value,
                  "all of the variants must be the same size.");
    return make_fdiagonal_impl<F, V, Vs...>::impl(make_index_sequence<decay_t<V>::size()>{});
  }
};

#if !defined(_MSC_VER) || _MSC_VER >= 1910
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
  inline static constexpr auto visit_alt(Visitor &&visitor, Vs &&... vs) DECLTYPE_AUTO_RETURN(
      base::at(base::make_fmatrix<Visitor &&, decltype(as_base(std::forward<Vs>(vs)))...>(),
               vs.index()...)(std::forward<Visitor>(visitor), as_base(std::forward<Vs>(vs))...))

      template <typename Visitor, typename... Vs>
      inline static constexpr auto visit_alt_at(std::size_t index, Visitor &&visitor, Vs &&... vs)
          DECLTYPE_AUTO_RETURN(base::at(
              base::make_fdiagonal<Visitor &&, decltype(as_base(std::forward<Vs>(vs)))...>(),
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
      return nostd::is_invocable<Visitor, Values...>::value;
    }
  };

  template <typename Visitor, typename... Values>
  struct visit_exhaustiveness_check
  {
    static_assert(visitor<Visitor>::template does_not_handle<Values...>(),
                  "`visit` requires the visitor to be exhaustive.");

    inline static constexpr auto invoke(Visitor &&visitor, Values &&... values)
        DECLTYPE_AUTO_RETURN(nostd::invoke(std::forward<Visitor>(visitor),
                                           std::forward<Values>(values)...))
  };

  template <typename Visitor>
  struct value_visitor
  {
    Visitor &&visitor_;

    template <typename... Alts>
    inline constexpr auto operator()(Alts &&... alts) const DECLTYPE_AUTO_RETURN(
        visit_exhaustiveness_check<Visitor, decltype((std::forward<Alts>(alts).value))...>::invoke(
            std::forward<Visitor>(visitor_),
            std::forward<Alts>(alts).value...))
  };

  template <typename Visitor>
  inline static constexpr auto make_value_visitor(Visitor &&visitor)
      AUTO_RETURN(value_visitor<Visitor>{std::forward<Visitor>(visitor)})

          public
      : template <typename Visitor, typename... Vs>
        inline static constexpr auto visit_alt(Visitor &&visitor, Vs &&... vs)
            DECLTYPE_AUTO_RETURN(alt::visit_alt(std::forward<Visitor>(visitor),
                                                std::forward<Vs>(vs).impl_...))

                template <typename Visitor, typename... Vs>
                inline static constexpr auto visit_alt_at(std::size_t index,
                                                          Visitor &&visitor,
                                                          Vs &&... vs)
                    DECLTYPE_AUTO_RETURN(alt::visit_alt_at(index,
                                                           std::forward<Visitor>(visitor),
                                                           std::forward<Vs>(vs).impl_...))

                        template <typename Visitor, typename... Vs>
                        inline static constexpr auto visit_value(Visitor &&visitor, Vs &&... vs)
                            DECLTYPE_AUTO_RETURN(
                                visit_alt(make_value_visitor(std::forward<Visitor>(visitor)),
                                          std::forward<Vs>(vs)...))

                                template <typename Visitor, typename... Vs>
                                inline static constexpr auto visit_value_at(std::size_t index,
                                                                            Visitor &&visitor,
                                                                            Vs &&... vs)
                                    DECLTYPE_AUTO_RETURN(visit_alt_at(
                                        index,
                                        make_value_visitor(std::forward<Visitor>(visitor)),
                                        std::forward<Vs>(vs)...))
};
}  // namespace visitation

template <typename... Ts>
using index_t = typename std::conditional<
    sizeof...(Ts) < (std::numeric_limits<unsigned char>::max)(),
    unsigned char,
    typename std::conditional<sizeof...(Ts) < (std::numeric_limits<unsigned short>::max)(),
                              unsigned short,
                              unsigned int>::type>::type;

template <Trait DestructibleTrait, typename... Ts>
class base
{
public:
  inline explicit constexpr base(valueless_t tag) noexcept
      : data_(tag), index_(static_cast<index_t<Ts...>>(-1))
  {}

  template <std::size_t I, typename... Args>
  inline explicit constexpr base(in_place_index_t<I>, Args &&... args)
      : data_(in_place_index_t<I>{}, std::forward<Args>(args)...), index_(I)
  {}

  inline constexpr bool valueless_by_exception() const noexcept
  {
    return index_ == static_cast<index_t<Ts...>>(-1);
  }

  inline constexpr std::size_t index() const noexcept
  {
    return valueless_by_exception() ? variant_npos : index_;
  }

protected:
  using data_t = recursive_union<DestructibleTrait, 0, Ts...>;

  friend inline constexpr base &as_base(base &b) { return b; }
  friend inline constexpr const base &as_base(const base &b) { return b; }
  friend inline constexpr base &&as_base(base &&b) { return std::move(b); }
  friend inline constexpr const base &&as_base(const base &&b) { return std::move(b); }

  friend inline constexpr data_t &data(base &b) { return b.data_; }
  friend inline constexpr const data_t &data(const base &b) { return b.data_; }
  friend inline constexpr data_t &&data(base &&b) { return std::move(b).data_; }
  friend inline constexpr const data_t &&data(const base &&b) { return std::move(b).data_; }

  inline static constexpr std::size_t size() { return sizeof...(Ts); }

  data_t data_;
  index_t<Ts...> index_;

  friend struct access::base;
  friend struct visitation::base;
};

struct dtor
{
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4100)
#endif
  template <typename Alt>
  inline void operator()(Alt &alt) const noexcept
  {
    alt.~Alt();
  }
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
};

template <typename Traits, Trait = Traits::destructible_trait>
class destructor;

#define OPENTELEMETRY_VARIANT_DESTRUCTOR(destructible_trait, definition, destroy)              \
  template <typename... Ts>                                                                    \
  class destructor<traits<Ts...>, destructible_trait> : public base<destructible_trait, Ts...> \
  {                                                                                            \
    using super = base<destructible_trait, Ts...>;                                             \
                                                                                               \
  public:                                                                                      \
    using super::super;                                                                        \
    using super::operator=;                                                                    \
                                                                                               \
    destructor(const destructor &) = default;                                                  \
    destructor(destructor &&)      = default;                                                  \
    definition destructor &operator=(const destructor &) = default;                            \
    destructor &operator=(destructor &&) = default;                                            \
                                                                                               \
  protected:                                                                                   \
    destroy                                                                                    \
  }

OPENTELEMETRY_VARIANT_DESTRUCTOR(
    Trait::TriviallyAvailable, ~destructor() = default;
    , inline void destroy() noexcept { this->index_ = static_cast<index_t<Ts...>>(-1); });

OPENTELEMETRY_VARIANT_DESTRUCTOR(
    Trait::Available,
    ~destructor() { destroy(); },
    inline void destroy() noexcept {
      if (!this->valueless_by_exception())
      {
        visitation::alt::visit_alt(dtor{}, *this);
      }
      this->index_ = static_cast<index_t<Ts...>>(-1);
    });

OPENTELEMETRY_VARIANT_DESTRUCTOR(Trait::Unavailable, ~destructor() = delete;
                                 , inline void destroy() noexcept  = delete;);

#undef OPENTELEMETRY_VARIANT_DESTRUCTOR

template <typename Traits>
class constructor : public destructor<Traits>
{
  using super = destructor<Traits>;

public:
  using super::super;
  using super::operator=;

protected:
  struct ctor
  {
    template <typename LhsAlt, typename RhsAlt>
    inline void operator()(LhsAlt &lhs_alt, RhsAlt &&rhs_alt) const
    {
      constructor::construct_alt(lhs_alt, std::forward<RhsAlt>(rhs_alt).value);
    }
  };

  template <std::size_t I, typename T, typename... Args>
  inline static T &construct_alt(alt<I, T> &a, Args &&... args)
  {
    auto *result = ::new (static_cast<void *>(std::addressof(a)))
        alt<I, T>(in_place_t{}, std::forward<Args>(args)...);
    return result->value;
  }

  template <typename Rhs>
  inline static void generic_construct(constructor &lhs, Rhs &&rhs)
  {
    lhs.destroy();
    if (!rhs.valueless_by_exception())
    {
      visitation::alt::visit_alt_at(rhs.index(), ctor{}, lhs, std::forward<Rhs>(rhs));
      lhs.index_ = rhs.index_;
    }
  }
};

template <typename Traits, Trait = Traits::move_constructible_trait>
class move_constructor;

#define OPENTELEMETRY_VARIANT_MOVE_CONSTRUCTOR(move_constructible_trait, definition) \
  template <typename... Ts>                                                          \
  class move_constructor<traits<Ts...>, move_constructible_trait>                    \
      : public constructor<traits<Ts...>>                                            \
  {                                                                                  \
    using super = constructor<traits<Ts...>>;                                        \
                                                                                     \
  public:                                                                            \
    using super::super;                                                              \
    using super::operator=;                                                          \
                                                                                     \
    move_constructor(const move_constructor &) = default;                            \
    definition ~move_constructor()             = default;                            \
    move_constructor &operator=(const move_constructor &) = default;                 \
    move_constructor &operator=(move_constructor &&) = default;                      \
  }

OPENTELEMETRY_VARIANT_MOVE_CONSTRUCTOR(Trait::TriviallyAvailable,
                                       move_constructor(move_constructor &&that) = default;);

OPENTELEMETRY_VARIANT_MOVE_CONSTRUCTOR(
    Trait::Available,
    move_constructor(move_constructor &&that) noexcept(
        all<std::is_nothrow_move_constructible<Ts>::value...>::value)
    : move_constructor(valueless_t{}) { this->generic_construct(*this, std::move(that)); });

OPENTELEMETRY_VARIANT_MOVE_CONSTRUCTOR(Trait::Unavailable,
                                       move_constructor(move_constructor &&) = delete;);

#undef OPENTELEMETRY_VARIANT_MOVE_CONSTRUCTOR

template <typename Traits, Trait = Traits::copy_constructible_trait>
class copy_constructor;

#define OPENTELEMETRY_VARIANT_COPY_CONSTRUCTOR(copy_constructible_trait, definition) \
  template <typename... Ts>                                                          \
  class copy_constructor<traits<Ts...>, copy_constructible_trait>                    \
      : public move_constructor<traits<Ts...>>                                       \
  {                                                                                  \
    using super = move_constructor<traits<Ts...>>;                                   \
                                                                                     \
  public:                                                                            \
    using super::super;                                                              \
    using super::operator=;                                                          \
                                                                                     \
    definition copy_constructor(copy_constructor &&) = default;                      \
    ~copy_constructor()                              = default;                      \
    copy_constructor &operator=(const copy_constructor &) = default;                 \
    copy_constructor &operator=(copy_constructor &&) = default;                      \
  }

OPENTELEMETRY_VARIANT_COPY_CONSTRUCTOR(Trait::TriviallyAvailable,
                                       copy_constructor(const copy_constructor &that) = default;);

OPENTELEMETRY_VARIANT_COPY_CONSTRUCTOR(
    Trait::Available, copy_constructor(const copy_constructor &that)
    : copy_constructor(valueless_t{}) { this->generic_construct(*this, that); });

OPENTELEMETRY_VARIANT_COPY_CONSTRUCTOR(Trait::Unavailable,
                                       copy_constructor(const copy_constructor &) = delete;);

#undef OPENTELEMETRY_VARIANT_COPY_CONSTRUCTOR

template <typename Traits>
class assignment : public copy_constructor<Traits>
{
  using super = copy_constructor<Traits>;

public:
  using super::super;
  using super::operator=;

  template <std::size_t I, typename... Args>
  inline /* auto & */ auto emplace(Args &&... args)
      -> decltype(this->construct_alt(access::base::get_alt<I>(*this), std::forward<Args>(args)...))
  {
    this->destroy();
    auto &result =
        this->construct_alt(access::base::get_alt<I>(*this), std::forward<Args>(args)...);
    this->index_ = I;
    return result;
  }

protected:
  template <typename That>
  struct assigner
  {
    template <typename ThisAlt, typename ThatAlt>
    inline void operator()(ThisAlt &this_alt, ThatAlt &&that_alt) const
    {
      self->assign_alt(this_alt, std::forward<ThatAlt>(that_alt).value);
    }
    assignment *self;
  };

  template <std::size_t I, typename T, typename Arg>
  inline void assign_alt(alt<I, T> &a, Arg &&arg)
  {
    if (this->index() == I)
    {
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)
#endif
      a.value = std::forward<Arg>(arg);
#ifdef _MSC_VER
#  pragma warning(pop)
#endif
    }
    else
    {
      struct
      {
        void operator()(std::true_type) const { this_->emplace<I>(std::forward<Arg>(arg_)); }
        void operator()(std::false_type) const { this_->emplace<I>(T(std::forward<Arg>(arg_))); }
        assignment *this_;
        Arg &&arg_;
      } impl{this, std::forward<Arg>(arg)};
      impl(bool_constant < std::is_nothrow_constructible<T, Arg>::value ||
           !std::is_nothrow_move_constructible<T>::value > {});
    }
  }

  template <typename That>
  inline void generic_assign(That &&that)
  {
    if (this->valueless_by_exception() && that.valueless_by_exception())
    {
      // do nothing.
    }
    else if (that.valueless_by_exception())
    {
      this->destroy();
    }
    else
    {
      visitation::alt::visit_alt_at(that.index(), assigner<That>{this}, *this,
                                    std::forward<That>(that));
    }
  }
};

template <typename Traits, Trait = Traits::move_assignable_trait>
class move_assignment;

#define OPENTELEMETRY_VARIANT_MOVE_ASSIGNMENT(move_assignable_trait, definition)                 \
  template <typename... Ts>                                                                      \
  class move_assignment<traits<Ts...>, move_assignable_trait> : public assignment<traits<Ts...>> \
  {                                                                                              \
    using super = assignment<traits<Ts...>>;                                                     \
                                                                                                 \
  public:                                                                                        \
    using super::super;                                                                          \
    using super::operator=;                                                                      \
                                                                                                 \
    move_assignment(const move_assignment &) = default;                                          \
    move_assignment(move_assignment &&)      = default;                                          \
    ~move_assignment()                       = default;                                          \
    move_assignment &operator=(const move_assignment &) = default;                               \
    definition                                                                                   \
  }

OPENTELEMETRY_VARIANT_MOVE_ASSIGNMENT(
    Trait::TriviallyAvailable, move_assignment &operator=(move_assignment &&that) = default;);

OPENTELEMETRY_VARIANT_MOVE_ASSIGNMENT(
    Trait::Available,
    move_assignment &
    operator=(move_assignment &&that) noexcept(
        all<(std::is_nothrow_move_constructible<Ts>::value &&
             std::is_nothrow_move_assignable<Ts>::value)...>::value) {
      this->generic_assign(std::move(that));
      return *this;
    });

OPENTELEMETRY_VARIANT_MOVE_ASSIGNMENT(Trait::Unavailable,
                                      move_assignment &operator=(move_assignment &&) = delete;);

#undef OPENTELEMETRY_VARIANT_MOVE_ASSIGNMENT

template <typename Traits, Trait = Traits::copy_assignable_trait>
class copy_assignment;

#define OPENTELEMETRY_VARIANT_COPY_ASSIGNMENT(copy_assignable_trait, definition) \
  template <typename... Ts>                                                      \
  class copy_assignment<traits<Ts...>, copy_assignable_trait>                    \
      : public move_assignment<traits<Ts...>>                                    \
  {                                                                              \
    using super = move_assignment<traits<Ts...>>;                                \
                                                                                 \
  public:                                                                        \
    using super::super;                                                          \
    using super::operator=;                                                      \
                                                                                 \
    copy_assignment(const copy_assignment &) = default;                          \
    copy_assignment(copy_assignment &&)      = default;                          \
    ~copy_assignment()                       = default;                          \
    definition copy_assignment &operator=(copy_assignment &&) = default;         \
  }

OPENTELEMETRY_VARIANT_COPY_ASSIGNMENT(
    Trait::TriviallyAvailable, copy_assignment &operator=(const copy_assignment &that) = default;);

OPENTELEMETRY_VARIANT_COPY_ASSIGNMENT(
    Trait::Available,
    copy_assignment &
    operator=(const copy_assignment &that) {
      this->generic_assign(that);
      return *this;
    });

OPENTELEMETRY_VARIANT_COPY_ASSIGNMENT(
    Trait::Unavailable, copy_assignment &operator=(const copy_assignment &) = delete;);

#undef OPENTELEMETRY_VARIANT_COPY_ASSIGNMENT
template <typename... Ts>
class impl : public copy_assignment<traits<Ts...>>
{
  using super = copy_assignment<traits<Ts...>>;

public:
  using super::super;
  using super::operator=;

  impl(const impl &) = default;
  impl(impl &&)      = default;
  ~impl()            = default;
  impl &operator=(const impl &) = default;
  impl &operator=(impl &&) = default;

  template <std::size_t I, typename Arg>
  inline void assign(Arg &&arg)
  {
    this->assign_alt(access::base::get_alt<I>(*this), std::forward<Arg>(arg));
  }

  inline void swap(impl &that)
  {
    if (this->valueless_by_exception() && that.valueless_by_exception())
    {
      // do nothing.
    }
    else if (this->index() == that.index())
    {
      visitation::alt::visit_alt_at(this->index(), swapper{}, *this, that);
    }
    else
    {
      impl *lhs = this;
      impl *rhs = std::addressof(that);
      if (lhs->move_nothrow() && !rhs->move_nothrow())
      {
        std::swap(lhs, rhs);
      }
      impl tmp(std::move(*rhs));
#if __EXCEPTIONS
      // EXTENSION: When the move construction of `lhs` into `rhs` throws
      // and `tmp` is nothrow move constructible then we move `tmp` back
      // into `rhs` and provide the strong exception safety guarantee.
      try
      {
        this->generic_construct(*rhs, std::move(*lhs));
      }
      catch (...)
      {
        if (tmp.move_nothrow())
        {
          this->generic_construct(*rhs, std::move(tmp));
        }
        throw;
      }
#else
      this->generic_construct(*rhs, std::move(*lhs));
#endif
      this->generic_construct(*lhs, std::move(tmp));
    }
  }

private:
  struct swapper
  {
    template <typename ThisAlt, typename ThatAlt>
    inline void operator()(ThisAlt &this_alt, ThatAlt &that_alt) const
    {
      using std::swap;
      swap(this_alt.value, that_alt.value);
    }
  };

  inline constexpr bool move_nothrow() const
  {
    return this->valueless_by_exception() ||
           std::array<bool, sizeof...(Ts)>{
               {std::is_nothrow_move_constructible<Ts>::value...}}[this->index()];
  }
};

template <typename From, typename To>
struct is_non_narrowing_convertible
{
  template <typename T>
  static std::true_type test(T(&&)[1]);

  template <typename T>
  static auto impl(int) -> decltype(test<T>({std::declval<From>()}));

  template <typename>
  static auto impl(...) -> std::false_type;

  static constexpr bool value = decltype(impl<To>(0))::value;
};

template <typename Arg,
          std::size_t I,
          typename T,
          bool     = std::is_arithmetic<T>::value,
          typename = void>
struct overload_leaf
{};

template <typename Arg, std::size_t I, typename T>
struct overload_leaf<Arg, I, T, false>
{
  using impl = size_constant<I> (*)(T);
  operator impl() const { return nullptr; };
};

template <typename Arg, std::size_t I, typename T>
struct overload_leaf<Arg,
                     I,
                     T,
                     true,
                     enable_if_t<std::is_same<remove_cvref_t<T>, bool>::value
                                     ? std::is_same<remove_cvref_t<Arg>, bool>::value
                                     :
#if defined(__clang__) || !defined(__GNUC__) || __GNUC__ >= 5
                                     is_non_narrowing_convertible<Arg, T>::value
#else
                                     std::is_convertible<Arg, T>::value
#endif
                                 >>
{
  using impl = size_constant<I> (*)(T);
  operator impl() const { return nullptr; };
};

template <typename Arg, typename... Ts>
struct overload_impl
{
private:
  template <typename>
  struct impl;

  template <std::size_t... Is>
  struct impl<index_sequence<Is...>> : overload_leaf<Arg, Is, Ts>...
  {};

public:
  using type = impl<index_sequence_for<Ts...>>;
};

template <typename Arg, typename... Ts>
using overload = typename overload_impl<Arg, Ts...>::type;

template <typename Arg, typename... Ts>
using best_match = invoke_result_t<overload<Arg, Ts...>, Arg>;

template <typename T>
struct is_in_place_index : std::false_type
{};

template <std::size_t I>
struct is_in_place_index<in_place_index_t<I>> : std::true_type
{};

template <typename T>
struct is_in_place_type : std::false_type
{};

template <typename T>
struct is_in_place_type<in_place_type_t<T>> : std::true_type
{};
}  // namespace detail

template <typename... Ts>
class variant
{
  static_assert(0 < sizeof...(Ts), "variant must consist of at least one alternative.");

  static_assert(detail::all<!std::is_array<Ts>::value...>::value,
                "variant can not have an array type as an alternative.");

  static_assert(detail::all<!std::is_reference<Ts>::value...>::value,
                "variant can not have a reference type as an alternative.");

  static_assert(detail::all<!std::is_void<Ts>::value...>::value,
                "variant can not have a void type as an alternative.");

public:
  template <typename Front = detail::type_pack_element_t<0, Ts...>,
            enable_if_t<std::is_default_constructible<Front>::value, int> = 0>
  inline constexpr variant() noexcept(std::is_nothrow_default_constructible<Front>::value)
      : impl_(in_place_index_t<0>{})
  {}

  variant(const variant &) = default;
  variant(variant &&)      = default;

  template <typename Arg,
            typename Decayed                                             = decay_t<Arg>,
            enable_if_t<!std::is_same<Decayed, variant>::value, int>     = 0,
            enable_if_t<!detail::is_in_place_index<Decayed>::value, int> = 0,
            enable_if_t<!detail::is_in_place_type<Decayed>::value, int>  = 0,
            std::size_t I = detail::best_match<Arg, Ts...>::value,
            typename T    = detail::type_pack_element_t<I, Ts...>,
            enable_if_t<std::is_constructible<T, Arg>::value, int> = 0>
  inline constexpr variant(Arg &&arg) noexcept(std::is_nothrow_constructible<T, Arg>::value)
      : impl_(in_place_index_t<I>{}, std::forward<Arg>(arg))
  {}

  template <std::size_t I,
            typename... Args,
            typename T = detail::type_pack_element_t<I, Ts...>,
            enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
  inline explicit constexpr variant(in_place_index_t<I>, Args &&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value)
      : impl_(in_place_index_t<I>{}, std::forward<Args>(args)...)
  {}

  template <
      std::size_t I,
      typename Up,
      typename... Args,
      typename T = detail::type_pack_element_t<I, Ts...>,
      enable_if_t<std::is_constructible<T, std::initializer_list<Up> &, Args...>::value, int> = 0>
  inline explicit constexpr variant(
      in_place_index_t<I>,
      std::initializer_list<Up> il,
      Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                              std::initializer_list<Up> &,
                                                              Args...>::value)
      : impl_(in_place_index_t<I>{}, il, std::forward<Args>(args)...)
  {}

  template <typename T,
            typename... Args,
            std::size_t I = detail::find_index_sfinae<T, Ts...>::value,
            enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
  inline explicit constexpr variant(in_place_type_t<T>, Args &&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value)
      : impl_(in_place_index_t<I>{}, std::forward<Args>(args)...)
  {}

  template <
      typename T,
      typename Up,
      typename... Args,
      std::size_t I = detail::find_index_sfinae<T, Ts...>::value,
      enable_if_t<std::is_constructible<T, std::initializer_list<Up> &, Args...>::value, int> = 0>
  inline explicit constexpr variant(
      in_place_type_t<T>,
      std::initializer_list<Up> il,
      Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                              std::initializer_list<Up> &,
                                                              Args...>::value)
      : impl_(in_place_index_t<I>{}, il, std::forward<Args>(args)...)
  {}

  ~variant() = default;

  variant &operator=(const variant &) = default;
  variant &operator=(variant &&) = default;

  template <
      typename Arg,
      enable_if_t<!std::is_same<decay_t<Arg>, variant>::value, int> = 0,
      std::size_t I    = detail::best_match<Arg, Ts...>::value,
      typename T       = detail::type_pack_element_t<I, Ts...>,
      enable_if_t<(std::is_assignable<T &, Arg>::value && std::is_constructible<T, Arg>::value),
                  int> = 0>
  inline variant &operator=(Arg &&arg) noexcept((std::is_nothrow_assignable<T &, Arg>::value &&
                                                 std::is_nothrow_constructible<T, Arg>::value))
  {
    impl_.template assign<I>(std::forward<Arg>(arg));
    return *this;
  }

  template <std::size_t I,
            typename... Args,
            typename T = detail::type_pack_element_t<I, Ts...>,
            enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
  inline T &emplace(Args &&... args)
  {
    return impl_.template emplace<I>(std::forward<Args>(args)...);
  }

  template <
      std::size_t I,
      typename Up,
      typename... Args,
      typename T = detail::type_pack_element_t<I, Ts...>,
      enable_if_t<std::is_constructible<T, std::initializer_list<Up> &, Args...>::value, int> = 0>
  inline T &emplace(std::initializer_list<Up> il, Args &&... args)
  {
    return impl_.template emplace<I>(il, std::forward<Args>(args)...);
  }

  template <typename T,
            typename... Args,
            std::size_t I = detail::find_index_sfinae<T, Ts...>::value,
            enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
  inline T &emplace(Args &&... args)
  {
    return impl_.template emplace<I>(std::forward<Args>(args)...);
  }

  template <
      typename T,
      typename Up,
      typename... Args,
      std::size_t I = detail::find_index_sfinae<T, Ts...>::value,
      enable_if_t<std::is_constructible<T, std::initializer_list<Up> &, Args...>::value, int> = 0>
  inline T &emplace(std::initializer_list<Up> il, Args &&... args)
  {
    return impl_.template emplace<I>(il, std::forward<Args>(args)...);
  }

  inline constexpr bool valueless_by_exception() const noexcept
  {
    return impl_.valueless_by_exception();
  }

  inline constexpr std::size_t index() const noexcept { return impl_.index(); }

  template <bool Dummy = true,
            enable_if_t<
                detail::all<Dummy,
                            (detail::dependent_type<std::is_move_constructible<Ts>, Dummy>::value &&
                             detail::dependent_type<is_swappable<Ts>, Dummy>::value)...>::value,
                int> = 0>
  inline void swap(variant &that) noexcept(
      detail::all<(std::is_nothrow_move_constructible<Ts>::value &&
                   is_nothrow_swappable<Ts>::value)...>::value)
  {
    impl_.swap(that.impl_);
  }

private:
  detail::impl<Ts...> impl_;

  friend struct detail::access::variant;
  friend struct detail::visitation::variant;
};

template <std::size_t I, typename... Ts>
inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept
{
  return v.index() == I;
}

template <typename T, typename... Ts>
inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept
{
  return holds_alternative<detail::find_index_checked<T, Ts...>::value>(v);
}

namespace detail
{
template <std::size_t I, typename V>
struct generic_get_impl
{
  constexpr generic_get_impl(int) noexcept {}

  constexpr auto operator()(V &&v) const
      AUTO_REFREF_RETURN(access::variant::get_alt<I>(std::forward<V>(v)).value)
};

template <std::size_t I, typename V>
inline constexpr auto generic_get(V &&v) AUTO_REFREF_RETURN(generic_get_impl<I, V>(
    holds_alternative<I>(v) ? 0 : (throw_bad_variant_access(), 0))(std::forward<V>(v)))
}  // namespace detail

template <std::size_t I, typename... Ts>
inline constexpr variant_alternative_t<I, variant<Ts...>> &get(variant<Ts...> &v)
{
  return detail::generic_get<I>(v);
}

template <std::size_t I, typename... Ts>
inline constexpr variant_alternative_t<I, variant<Ts...>> &&get(variant<Ts...> &&v)
{
  return detail::generic_get<I>(std::move(v));
}

template <std::size_t I, typename... Ts>
inline constexpr const variant_alternative_t<I, variant<Ts...>> &get(const variant<Ts...> &v)
{
  return detail::generic_get<I>(v);
}

template <std::size_t I, typename... Ts>
inline constexpr const variant_alternative_t<I, variant<Ts...>> &&get(const variant<Ts...> &&v)
{
  return detail::generic_get<I>(std::move(v));
}

template <typename T, typename... Ts>
inline constexpr T &get(variant<Ts...> &v)
{
  return get<detail::find_index_checked<T, Ts...>::value>(v);
}

template <typename T, typename... Ts>
inline constexpr T &&get(variant<Ts...> &&v)
{
  return get<detail::find_index_checked<T, Ts...>::value>(std::move(v));
}

template <typename T, typename... Ts>
inline constexpr const T &get(const variant<Ts...> &v)
{
  return get<detail::find_index_checked<T, Ts...>::value>(v);
}

template <typename T, typename... Ts>
inline constexpr const T &&get(const variant<Ts...> &&v)
{
  return get<detail::find_index_checked<T, Ts...>::value>(std::move(v));
}

namespace detail
{

template <std::size_t I, typename V>
inline constexpr /* auto * */ auto generic_get_if(V *v) noexcept
    AUTO_RETURN(v &&holds_alternative<I>(*v) ? std::addressof(access::variant::get_alt<I>(*v).value)
                                             : nullptr)

}  // namespace detail

template <std::size_t I, typename... Ts>
inline constexpr add_pointer_t<variant_alternative_t<I, variant<Ts...>>> get_if(
    variant<Ts...> *v) noexcept
{
  return detail::generic_get_if<I>(v);
}

template <std::size_t I, typename... Ts>
inline constexpr add_pointer_t<const variant_alternative_t<I, variant<Ts...>>> get_if(
    const variant<Ts...> *v) noexcept
{
  return detail::generic_get_if<I>(v);
}

template <typename T, typename... Ts>
inline constexpr add_pointer_t<T> get_if(variant<Ts...> *v) noexcept
{
  return get_if<detail::find_index_checked<T, Ts...>::value>(v);
}

template <typename T, typename... Ts>
inline constexpr add_pointer_t<const T> get_if(const variant<Ts...> *v) noexcept
{
  return get_if<detail::find_index_checked<T, Ts...>::value>(v);
}

namespace detail
{
template <typename RelOp>
struct convert_to_bool
{
  template <typename Lhs, typename Rhs>
  inline constexpr bool operator()(Lhs &&lhs, Rhs &&rhs) const
  {
    static_assert(std::is_convertible<invoke_result_t<RelOp, Lhs, Rhs>, bool>::value,
                  "relational operators must return a type"
                  " implicitly convertible to bool");
    return nostd::invoke(RelOp{}, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
};
}  // namespace detail

template <typename... Ts>
inline constexpr bool operator==(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using equal_to = detail::convert_to_bool<detail::equal_to>;
  return lhs.index() == rhs.index() && (lhs.valueless_by_exception() ||
                                        variant::visit_value_at(lhs.index(), equal_to{}, lhs, rhs));
}

template <typename... Ts>
inline constexpr bool operator!=(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using not_equal_to = detail::convert_to_bool<detail::not_equal_to>;
  return lhs.index() != rhs.index() ||
         (!lhs.valueless_by_exception() &&
          variant::visit_value_at(lhs.index(), not_equal_to{}, lhs, rhs));
}

template <typename... Ts>
inline constexpr bool operator<(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using less = detail::convert_to_bool<detail::less>;
  return !rhs.valueless_by_exception() &&
         (lhs.valueless_by_exception() || lhs.index() < rhs.index() ||
          (lhs.index() == rhs.index() && variant::visit_value_at(lhs.index(), less{}, lhs, rhs)));
}

template <typename... Ts>
inline constexpr bool operator>(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using greater = detail::convert_to_bool<detail::greater>;
  return !lhs.valueless_by_exception() &&
         (rhs.valueless_by_exception() || lhs.index() > rhs.index() ||
          (lhs.index() == rhs.index() &&
           variant::visit_value_at(lhs.index(), greater{}, lhs, rhs)));
}

template <typename... Ts>
inline constexpr bool operator<=(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using less_equal = detail::convert_to_bool<detail::less_equal>;
  return lhs.valueless_by_exception() ||
         (!rhs.valueless_by_exception() &&
          (lhs.index() < rhs.index() ||
           (lhs.index() == rhs.index() &&
            variant::visit_value_at(lhs.index(), less_equal{}, lhs, rhs))));
}

template <typename... Ts>
inline constexpr bool operator>=(const variant<Ts...> &lhs, const variant<Ts...> &rhs)
{
  using detail::visitation::variant;
  using greater_equal = detail::convert_to_bool<detail::greater_equal>;
  return rhs.valueless_by_exception() ||
         (!lhs.valueless_by_exception() &&
          (lhs.index() > rhs.index() ||
           (lhs.index() == rhs.index() &&
            variant::visit_value_at(lhs.index(), greater_equal{}, lhs, rhs))));
}

struct monostate
{};

inline constexpr bool operator<(monostate, monostate) noexcept
{
  return false;
}

inline constexpr bool operator>(monostate, monostate) noexcept
{
  return false;
}

inline constexpr bool operator<=(monostate, monostate) noexcept
{
  return true;
}

inline constexpr bool operator>=(monostate, monostate) noexcept
{
  return true;
}

inline constexpr bool operator==(monostate, monostate) noexcept
{
  return true;
}

inline constexpr bool operator!=(monostate, monostate) noexcept
{
  return false;
}

namespace detail
{

template <std::size_t N>
inline constexpr bool all_of_impl(const std::array<bool, N> &bs, std::size_t idx)
{
  return idx >= N || (bs[idx] && all_of_impl(bs, idx + 1));
}

template <std::size_t N>
inline constexpr bool all_of(const std::array<bool, N> &bs)
{
  return all_of_impl(bs, 0);
}

}  // namespace detail

template <typename Visitor, typename... Vs>
inline constexpr auto visit(Visitor &&visitor, Vs &&... vs) DECLTYPE_AUTO_RETURN(
    (detail::all_of(std::array<bool, sizeof...(Vs)>{{!vs.valueless_by_exception()...}})
         ? (void)0
         : throw_bad_variant_access()),
    detail::visitation::variant::visit_value(std::forward<Visitor>(visitor),
                                             std::forward<Vs>(vs)...)) template <typename... Ts>
inline auto swap(variant<Ts...> &lhs, variant<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
    -> decltype(lhs.swap(rhs))
{
  lhs.swap(rhs);
}
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

#undef AUTO_RETURN

#undef AUTO_REFREF_RETURN

#undef DECLTYPE_AUTO_RETURN
