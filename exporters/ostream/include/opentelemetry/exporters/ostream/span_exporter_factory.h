// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <memory>

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class SpanExporter;
}  // namespace trace
}  // namespace sdk

namespace exporter
{
namespace trace
{

class OStreamSpanExporterFactory
{
public:
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(std::ostream &sout);
};

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
