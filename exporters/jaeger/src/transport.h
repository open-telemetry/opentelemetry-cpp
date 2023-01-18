// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/version.h>

#include <opentelemetry/exporters/jaeger/thrift_include_prefix.h>

#include <jaeger_types.h>

#include <opentelemetry/exporters/jaeger/thrift_include_suffix.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

class Transport
{
public:
  Transport()          = default;
  virtual ~Transport() = default;

  virtual int EmitBatch(const thrift::Batch &batch) = 0;
  virtual uint32_t MaxPacketSize() const            = 0;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
