// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/configuration/configuration_factory.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

namespace
{
void InitTracer()
{
  std::string config_file = "config.yaml";
  auto model = opentelemetry::sdk::configuration::ConfigurationFactory::Parse(config_file);
}

void CleanupTracer()
{
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();

  CleanupTracer();
}
