// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/zpages/tracez_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{
namespace trace_sdk = opentelemetry::sdk::trace;

void TracezSpanProcessor::OnStart(trace_sdk::Recordable &span,
                                  const opentelemetry::trace::SpanContext &parent_context) noexcept
{
  shared_data_->OnStart(static_cast<ThreadsafeSpanData *>(&span));
}

void TracezSpanProcessor::OnEnd(std::unique_ptr<trace_sdk::Recordable> &&span) noexcept
{
  shared_data_->OnEnd(
      std::unique_ptr<ThreadsafeSpanData>(static_cast<ThreadsafeSpanData *>(span.release())));
}

}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
