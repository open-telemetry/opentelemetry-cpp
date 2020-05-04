#pragma once

#include <atomic>
#include <memory>
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
/**
 * A wrapper to provide atomic shared pointers.
 *
 * This wrapper relies on std::atomic for gcc 4.8 and C++20, while using
 * specializations of std::atomic_store and std::atomic_load in all other
 * instances.
 */
#if __cplusplus > 201703L || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8))
template <class T>
class AtomicSharedPtr
{
public:
  explicit AtomicSharedPtr(std::shared_ptr<T> ptr) noexcept : ptr_{std::move(ptr)} {}

  void store(const std::shared_ptr<T> &other) noexcept
  {
    ptr_.store(other, std::memory_order_release);
  }

  std::shared_ptr<T> load() const noexcept { return ptr_.load(std::memory_order_acquire); }

private:
  std::atomic<std::shared_ptr<T>> ptr_;
};
#else
template <class T>
class AtomicSharedPtr
{
public:
  explicit AtomicSharedPtr(std::shared_ptr<T> ptr) noexcept : ptr_{std::move(ptr)} {}

  void store(const std::shared_ptr<T> &other) noexcept { std::atomic_store(&ptr_, other); }

  std::shared_ptr<T> load() const noexcept { return std::atomic_load(&ptr_); }

private:
  std::shared_ptr<T> ptr_;
};
#endif
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
