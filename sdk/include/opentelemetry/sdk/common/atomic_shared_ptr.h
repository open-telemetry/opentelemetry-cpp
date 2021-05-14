// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/**
 * A wrapper to provide atomic shared pointers.
 *
 * This wrapper relies on a mutex for gcc 4.8, and specializations of
 * std::atomic_store and std::atomic_load for all other instances.
 */
#if (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8))
template <class T>
class AtomicSharedPtr
{
public:
  explicit AtomicSharedPtr(std::shared_ptr<T> ptr) noexcept : ptr_{std::move(ptr)} {}

  void store(const std::shared_ptr<T> &other) noexcept
  {
    std::lock_guard<std::mutex> lock_guard{mu_};
    ptr_ = other;
  }

  std::shared_ptr<T> load() const noexcept
  {
    std::lock_guard<std::mutex> lock_guard{mu_};
    return ptr_;
  }

private:
  std::shared_ptr<T> ptr_;
  mutable std::mutex mu_;
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
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
