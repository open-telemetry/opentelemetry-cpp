// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <atomic>

#include "opentelemetry/version.h"
#include "src/metrics/collection_cancellation.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace
{
thread_local const std::atomic<bool> *current_stop_requested = nullptr;
}  // namespace

bool CollectionCancellation::IsRequested() noexcept
{
  return current_stop_requested != nullptr &&
         current_stop_requested->load(std::memory_order_acquire);
}

CollectionCancellation::Scope::Scope(const std::atomic<bool> &stop_requested) noexcept
    : previous_(current_stop_requested)
{
  current_stop_requested = &stop_requested;
}

CollectionCancellation::Scope::~Scope() noexcept
{
  current_stop_requested = previous_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
