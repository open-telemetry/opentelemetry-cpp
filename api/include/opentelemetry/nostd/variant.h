#pragma once

#include <array>
#include <exception>
#include <limits>

#include "opentelemetry/nostd/detail/all.h"
#include "opentelemetry/nostd/detail/find_index.h"
#include "opentelemetry/nostd/detail/recursive_union.h"
#include "opentelemetry/nostd/detail/trait.h"
#include "opentelemetry/nostd/detail/type_pack_element.h"
#include "opentelemetry/nostd/detail/variant_alternative.h"
#include "opentelemetry/nostd/detail/variant_fwd.h"
#include "opentelemetry/nostd/detail/variant_size.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

#define AUTO auto
#define AUTO_RETURN(...) \
  ->decay_t<decltype(__VA_ARGS__)> { return __VA_ARGS__; }

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
  using dispatch_result_t =
      decltype(invoke(std::declval<Visitor>(), access::base::get_alt<0>(std::declval<Vs>())...));

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
      using Actual =
          decltype(invoke(std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<Is>(std::forward<Vs>(vs))...);
    }

    template <typename...>
    struct impl;

    template <std::size_t... Is>
    struct impl<index_sequence<Is...>>
    {
      inline constexpr AUTO operator()() const AUTO_RETURN(&dispatch<Is...>)
    };

    template <typename Is, std::size_t... Js, typename... Ls>
    struct impl<Is, index_sequence<Js...>, Ls...>
    {
      inline constexpr AUTO operator()() const
          AUTO_RETURN(make_farray(impl<detail::index_sequence_push_back_t<Is, Js>, Ls...>{}()...))
    };
  };

  template <typename F, typename... Vs>
  inline static constexpr AUTO make_fmatrix() AUTO_RETURN(
      typename make_fmatrix_impl<F, Vs...>::
          template impl<index_sequence<>, make_index_sequence<decay_t<Vs>::size()>...>{}())

      template <typename F, typename... Vs>
      struct make_fdiagonal_impl
  {
    template <std::size_t I>
    inline static constexpr dispatch_result_t<F, Vs...> dispatch(F &&f, Vs &&... vs)
    {
      using Expected = dispatch_result_t<F, Vs...>;
      using Actual =
          decltype(invoke(std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...));
      return visit_return_type_check<Expected, Actual>::invoke(
          std::forward<F>(f), access::base::get_alt<I>(std::forward<Vs>(vs))...);
    }

    template <std::size_t... Is>
    inline static constexpr AUTO impl(index_sequence<Is...>)
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
      return is_invocable<Visitor, Values...>::value;
    }
  };

  template <typename Visitor, typename... Values>
  struct visit_exhaustiveness_check
  {
    static_assert(visitor<Visitor>::template does_not_handle<Values...>(),
                  "`visit` requires the visitor to be exhaustive.");

    inline static constexpr DECLTYPE_AUTO invoke(Visitor &&visitor, Values &&... values)
        DECLTYPE_AUTO_RETURN(invoke(std::forward<Visitor>(visitor),
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

#define MPARK_VARIANT_DESTRUCTOR(destructible_trait, definition, destroy)                      \
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

MPARK_VARIANT_DESTRUCTOR(
    Trait::TriviallyAvailable, ~destructor() = default;
    , inline void destroy() noexcept { this->index_ = static_cast<index_t<Ts...>>(-1); });

MPARK_VARIANT_DESTRUCTOR(
    Trait::Available,
    ~destructor() { destroy(); },
    inline void destroy() noexcept {
      if (!this->valueless_by_exception())
      {
        visitation::alt::visit_alt(dtor{}, *this);
      }
      this->index_ = static_cast<index_t<Ts...>>(-1);
    });

MPARK_VARIANT_DESTRUCTOR(Trait::Unavailable, ~destructor() = delete;
                         , inline void destroy() noexcept  = delete;);

#undef MPARK_VARIANT_DESTRUCTOR

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

#define MPARK_VARIANT_MOVE_CONSTRUCTOR(move_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class move_constructor<traits<Ts...>, move_constructible_trait>            \
      : public constructor<traits<Ts...>>                                    \
  {                                                                          \
    using super = constructor<traits<Ts...>>;                                \
                                                                             \
  public:                                                                    \
    using super::super;                                                      \
    using super::operator=;                                                  \
                                                                             \
    move_constructor(const move_constructor &) = default;                    \
    definition ~move_constructor()             = default;                    \
    move_constructor &operator=(const move_constructor &) = default;         \
    move_constructor &operator=(move_constructor &&) = default;              \
  }

MPARK_VARIANT_MOVE_CONSTRUCTOR(Trait::TriviallyAvailable,
                               move_constructor(move_constructor &&that) = default;);

MPARK_VARIANT_MOVE_CONSTRUCTOR(
    Trait::Available,
    move_constructor(move_constructor &&that) noexcept(
        all<std::is_nothrow_move_constructible<Ts>::value...>::value)
    : move_constructor(valueless_t{}) { this->generic_construct(*this, std::move(that)); });

MPARK_VARIANT_MOVE_CONSTRUCTOR(Trait::Unavailable, move_constructor(move_constructor &&) = delete;);

#undef MPARK_VARIANT_MOVE_CONSTRUCTOR

    template <typename Traits, Trait = Traits::copy_constructible_trait>
    class copy_constructor;

#define MPARK_VARIANT_COPY_CONSTRUCTOR(copy_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class copy_constructor<traits<Ts...>, copy_constructible_trait>            \
      : public move_constructor<traits<Ts...>>                               \
  {                                                                          \
    using super = move_constructor<traits<Ts...>>;                           \
                                                                             \
  public:                                                                    \
    using super::super;                                                      \
    using super::operator=;                                                  \
                                                                             \
    definition copy_constructor(copy_constructor &&) = default;              \
    ~copy_constructor()                              = default;              \
    copy_constructor &operator=(const copy_constructor &) = default;         \
    copy_constructor &operator=(copy_constructor &&) = default;              \
  }

    MPARK_VARIANT_COPY_CONSTRUCTOR(
        Trait::TriviallyAvailable,
        copy_constructor(const copy_constructor &that) = default;);

    MPARK_VARIANT_COPY_CONSTRUCTOR(
        Trait::Available,
        copy_constructor(const copy_constructor &that)
            : copy_constructor(valueless_t{}) {
          this->generic_construct(*this, that);
        });

    MPARK_VARIANT_COPY_CONSTRUCTOR(
        Trait::Unavailable,
        copy_constructor(const copy_constructor &) = delete;);

#undef MPARK_VARIANT_COPY_CONSTRUCTOR

    template <typename Traits>
    class assignment : public copy_constructor<Traits> {
      using super = copy_constructor<Traits>;

      public:
      using super::super;
      using super::operator=;

      template <std::size_t I, typename... Args>
      inline /* auto & */ auto emplace(Args &&... args)
          -> decltype(this->construct_alt(access::base::get_alt<I>(*this),
                                          std::forward<Args>(args)...)) {
        this->destroy();
        auto &result = this->construct_alt(access::base::get_alt<I>(*this),
                                           std::forward<Args>(args)...);
        this->index_ = I;
        return result;
      }

      protected:
      template <typename That>
      struct assigner {
        template <typename ThisAlt, typename ThatAlt>
        inline void operator()(ThisAlt &this_alt, ThatAlt &&that_alt) const {
          self->assign_alt(this_alt, std::forward<ThatAlt>(that_alt).value);
        }
        assignment *self;
      };

      template <std::size_t I, typename T, typename Arg>
      inline void assign_alt(alt<I, T> &a, Arg &&arg) {
        if (this->index() == I) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
          a.value = std::forward<Arg>(arg);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        } else {
          struct {
            void operator()(std::true_type) const {
              this_->emplace<I>(std::forward<Arg>(arg_));
            }
            void operator()(std::false_type) const {
              this_->emplace<I>(T(std::forward<Arg>(arg_)));
            }
            assignment *this_;
            Arg &&arg_;
          } impl{this, std::forward<Arg>(arg)};
          impl(bool_constant < std::is_nothrow_constructible<T, Arg>::value ||
               !std::is_nothrow_move_constructible<T>::value > {});
        }
      }

      template <typename That>
      inline void generic_assign(That &&that) {
        if (this->valueless_by_exception() && that.valueless_by_exception()) {
          // do nothing.
        } else if (that.valueless_by_exception()) {
          this->destroy();
        } else {
          visitation::alt::visit_alt_at(
              that.index(),
              assigner<That>{this}
              ,
              *this,
              std::forward<That>(that));
        }
      }
    };
}  // namespace detail

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

#undef AUTO
#undef AUTO_RETURN

#undef AUTO_REFREF
#undef AUTO_REFREF_RETURN

#undef DECLTYPE_AUTO
#undef DECLTYPE_AUTO_RETURN
