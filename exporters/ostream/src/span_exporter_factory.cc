// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"

namespace trace_sdk = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

nostd::unique_ptr<trace_sdk::SpanExporter> OStreamSpanExporterFactory::Build()
{
  return Build(std::cout);
};

nostd::unique_ptr<trace_sdk::SpanExporter> OStreamSpanExporterFactory::Build(std::ostream &sout)
{
  nostd::unique_ptr<trace_sdk::SpanExporter> exporter(new OStreamSpanExporter(sout));
  return std::move(exporter);
}

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
