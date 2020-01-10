#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace opentelemetry
{
namespace nostd
{
/**
 * Provide a simplified port of std::unique_ptr that has ABI stability.
 *
 * Note: This implementation doesn't allow for a custom deleter or array specializations.
 */
template <class T>
class unique_ptr
{
public:
  unique_ptr() noexcept : ptr_{nullptr} {}

  unique_ptr(nullptr_t) noexcept : ptr_{nullptr} {}

  explicit unique_ptr(T *ptr) noexcept : ptr_{ptr} {}

  unique_ptr(unique_ptr &&other) noexcept : ptr_{other.release()} {}

  template <class U,
            typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = nullptr>
  unique_ptr(unique_ptr<U> &&other) noexcept : ptr_{other.release()}
  {}

  template <class U,
            typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = nullptr>
  unique_ptr(std::unique_ptr<U> &&other) noexcept : ptr_{other.release()}
  {}

  ~unique_ptr() { reset(); }

  unique_ptr &operator=(unique_ptr &&other) noexcept
  {
    reset(other.release());
    return *this;
  }

  unique_ptr &operator=(nullptr_t) noexcept
  {
    reset();
    return *this;
  }

  template <class U,
            typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = nullptr>
  unique_ptr &operator=(unique_ptr<U> &&other) noexcept
  {
    reset(other.release());
    return *this;
  }

  template <class U,
            typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = nullptr>
  unique_ptr &operator=(std::unique_ptr<U> &&other) noexcept
  {
    reset(other.release());
    return *this;
  }

  operator std::unique_ptr<T>() && noexcept { return std::unique_ptr<T>{release()}; }

  operator bool() const noexcept { return ptr_ != nullptr; }

  T &operator*() const noexcept { return *ptr_; }

  T *operator->() const noexcept { return get(); }

  T *get() const noexcept { return ptr_; }

  void reset(T *ptr = nullptr) noexcept
  {
    if (ptr_ != nullptr)
    {
      delete ptr_;
    }
    ptr_ = ptr;
  }

  T *release() noexcept
  {
    auto result = ptr_;
    ptr_        = nullptr;
    return result;
  }

  void swap(unique_ptr &other) noexcept { std::swap(ptr_, other.ptr_); }

private:
  T *ptr_;
};

template <class T1, class T2>
bool operator==(const unique_ptr<T1> &lhs, const unique_ptr<T2> &rhs) noexcept
{
  return lhs.get() == rhs.get();
}

template <class T>
bool operator==(const unique_ptr<T> &lhs, nullptr_t) noexcept
{
  return lhs.get() == nullptr;
}

template <class T>
bool operator==(nullptr_t, const unique_ptr<T> &rhs) noexcept
{
  return nullptr == rhs.get();
}

template <class T1, class T2>
bool operator!=(const unique_ptr<T1> &lhs, const unique_ptr<T2> &rhs) noexcept
{
  return lhs.get() != rhs.get();
}

template <class T>
bool operator!=(const unique_ptr<T> &lhs, nullptr_t) noexcept
{
  return lhs.get() != nullptr;
}

template <class T>
bool operator!=(nullptr_t, const unique_ptr<T> &rhs) noexcept
{
  return nullptr != rhs.get();
}
}  // namespace nostd
}  // namespace opentelemetry
