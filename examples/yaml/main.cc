// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/configuration_factory.h"
#include "opentelemetry/sdk/init/configured_sdk.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

std::unique_ptr<opentelemetry::sdk::init::ConfiguredSdk> sdk;

namespace
{
void InitOtel()
{
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
