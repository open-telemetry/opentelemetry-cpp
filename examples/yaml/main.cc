// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/init/configured_sdk.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

#include "opentelemetry/exporters/otlp/otlp_builder.h"
#include "opentelemetry/exporters/ostream/console_builder.h"
#include "opentelemetry/exporters/zipkin/zipkin_builder.h"


std::unique_ptr<opentelemetry::sdk::init::ConfiguredSdk> sdk;

namespace
{
void InitOtel()
{
  opentelemetry::exporter::otlp::OtlpBuilder::Register(nullptr);
  opentelemetry::exporter::trace::ConsoleBuilder::Register(nullptr);
  opentelemetry::exporter::zipkin::ZipkinBuilder::Register(nullptr);

  // See
  // https://github.com/open-telemetry/opentelemetry-configuration/blob/main/examples/kitchen-sink.yaml
  std::string config_file = "config.yaml";
  auto model = opentelemetry::sdk::configuration::ConfigurationFactory::ParseFile(config_file);




  sdk = opentelemetry::sdk::init::ConfiguredSdk::Create(model);

  if (sdk != nullptr)
  {
    sdk->Install();
  }
}

void CleanupOtel()
{
  if (sdk != nullptr)
  {
    sdk->UnInstall();
  }
  sdk.reset(nullptr);
}
}  // namespace

int main()
{
  InitOtel();

  foo_library();

  CleanupOtel();
}
