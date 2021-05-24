// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

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
