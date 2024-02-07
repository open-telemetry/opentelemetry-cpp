// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/registry.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

#include "opentelemetry/exporters/ostream/console_builder.h"
#include "opentelemetry/exporters/otlp/otlp_builder.h"
#include "opentelemetry/exporters/zipkin/zipkin_builder.h"

std::unique_ptr<opentelemetry::sdk::init::ConfiguredSdk> sdk;

namespace
{
void InitOtel()
{
  /* 1 - Create a registry */

  std::shared_ptr<opentelemetry::sdk::init::Registry> registry(
      new opentelemetry::sdk::init::Registry);

  /* 2 - Populate the registry with the core components supported */

  opentelemetry::exporter::otlp::OtlpBuilder::Register(registry.get());
  opentelemetry::exporter::trace::ConsoleBuilder::Register(registry.get());
  opentelemetry::exporter::zipkin::ZipkinBuilder::Register(registry.get());

  /* 3 - Populate the registry with external extensions plugins */

  /* 4 - Parse a config.yaml */

  // See
  // https://github.com/open-telemetry/opentelemetry-configuration/blob/main/examples/kitchen-sink.yaml
  std::string config_file = "config.yaml";
  auto model = opentelemetry::sdk::configuration::ConfigurationFactory::ParseFile(config_file);

  /* 5 - Build the SDK from the parsed config.yaml */

  sdk = opentelemetry::sdk::init::ConfiguredSdk::Create(registry, model);

  /* 6 - Deploy the SDK */

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
