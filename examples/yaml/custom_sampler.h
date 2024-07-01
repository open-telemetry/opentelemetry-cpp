// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/init/extension_sampler_builder.h"
#include "opentelemetry/sdk/trace/sampler.h"

class CustomSampler : public opentelemetry::sdk::trace::Sampler
{
public:
  CustomSampler(const std::string & /* comment */) {}
  ~CustomSampler() override = default;

  opentelemetry::sdk::trace::SamplingResult ShouldSample(
      const opentelemetry::trace::SpanContext &parent_context,
      opentelemetry::trace::TraceId trace_id,
      opentelemetry::nostd::string_view name,
      opentelemetry::trace::SpanKind span_kind,
      const opentelemetry::common::KeyValueIterable &attributes,
      const opentelemetry::trace::SpanContextKeyValueIterable &links) noexcept override;

  opentelemetry::nostd::string_view GetDescription() const noexcept override;
};
