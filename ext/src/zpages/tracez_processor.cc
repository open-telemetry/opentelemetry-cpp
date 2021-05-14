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

#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

void TracezSpanProcessor::OnStart(opentelemetry::sdk::trace::Recordable &span,
                                  const opentelemetry::trace::SpanContext &parent_context) noexcept
{
  shared_data_->OnStart(static_cast<ThreadsafeSpanData *>(&span));
}

void TracezSpanProcessor::OnEnd(
    std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept
{
  shared_data_->OnEnd(
      std::unique_ptr<ThreadsafeSpanData>(static_cast<ThreadsafeSpanData *>(span.release())));
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
