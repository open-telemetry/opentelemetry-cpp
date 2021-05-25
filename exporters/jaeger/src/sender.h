// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/version.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

class Sender
{
public:
  Sender()          = default;
  virtual ~Sender() = default;

  virtual int Append(std::unique_ptr<Recordable> &&span) = 0;

  virtual int Flush() = 0;

  virtual void Close() = 0;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
