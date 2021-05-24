// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_shared_data.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

void TracezSharedData::OnStart(ThreadsafeSpanData *span) noexcept
{
  std::lock_guard<std::mutex> lock(mtx_);
  spans_.running.insert(span);
}

void TracezSharedData::OnEnd(std::unique_ptr<ThreadsafeSpanData> &&span) noexcept
{
  std::lock_guard<std::mutex> lock(mtx_);
  auto span_it = spans_.running.find(span.get());
  if (span_it != spans_.running.end())
  {
    spans_.running.erase(span_it);
    spans_.completed.push_back(std::unique_ptr<ThreadsafeSpanData>(span.release()));
  }
}

TracezSharedData::CollectedSpans TracezSharedData::GetSpanSnapshot() noexcept
{
  CollectedSpans snapshot;
  std::lock_guard<std::mutex> lock(mtx_);
  snapshot.running   = spans_.running;
  snapshot.completed = std::move(spans_.completed);
  spans_.completed.clear();
  return snapshot;
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
