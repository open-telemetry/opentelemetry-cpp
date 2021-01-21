// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "opentelemetry/exporters/jaeger/jaeger_exporter.h"
#include "opentelemetry/exporters/jaeger/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

JaegerExporter::JaegerExporter(const JaegerExporterOptions &options)
    : options_(options)
{
  InitializeEndpoint();
}

JaegerExporter::JaegerExporter() : JaegerExporter(JaegerExporterOptions())
{

}

std::unique_ptr<std::trace::Recordable> JaegerExporter::MakeRecordable() noexcept
{
  return std::unique<sdk::trace::Recordable>(new Recordable);
}

std::trace::ExportResult JaegerExporter::Export(
    const nostd::span<std::unique_ptr<std::trace::Recordable>> &spans) noexcept
{
  if (is_shutdown_)
  {
      return sdk::trace::ExportResult::kFailuer;
  }
  return sdk::trace::ExportResult::kSuccess;
}

void ZipkinExporter::InitializeLocalEndpoint()
{

}

}  // namespace jaeger
}  // namespace exporter


