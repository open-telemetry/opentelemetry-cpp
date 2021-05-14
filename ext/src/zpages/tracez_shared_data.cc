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
