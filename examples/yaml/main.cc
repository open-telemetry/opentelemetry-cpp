// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_factory.h"
#include "opentelemetry/sdk/init/configured_sdk.h"
#include "opentelemetry/sdk/init/registry.h"

#include "custom_sampler_builder.h"
#include "custom_span_exporter_builder.h"
#include "custom_span_processor_builder.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

#include "opentelemetry/exporters/ostream/console_builder.h"

#ifdef OTEL_HAVE_OTLP
#  include "opentelemetry/exporters/otlp/otlp_builder.h"
#endif

#ifdef OTEL_HAVE_ZIPKIN
#  include "opentelemetry/exporters/zipkin/zipkin_builder.h"
#endif

#include <iostream>
#include <string>

static bool opt_help              = false;
static std::string yaml_file_path = "";

std::unique_ptr<opentelemetry::sdk::init::ConfiguredSdk> sdk;

namespace
{

void InitOtel(const std::string &config_file)
{
  auto level = opentelemetry::sdk::common::internal_log::LogLevel::Debug;

  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(level);

  /* 1 - Create a registry */

  std::shared_ptr<opentelemetry::sdk::init::Registry> registry(
      new opentelemetry::sdk::init::Registry);

  /* 2 - Populate the registry with the core components supported */

  opentelemetry::exporter::trace::ConsoleBuilder::Register(registry.get());

#ifdef OTEL_HAVE_OTLP
  opentelemetry::exporter::otlp::OtlpBuilder::Register(registry.get());
#endif

#ifdef OTEL_HAVE_ZIPKIN
  opentelemetry::exporter::zipkin::ZipkinBuilder::Register(registry.get());
#endif

  /* 3 - Populate the registry with external extensions plugins */

  CustomSamplerBuilder::Register(registry.get());
  CustomSpanExporterBuilder::Register(registry.get());
  CustomSpanProcessorBuilder::Register(registry.get());

  /* 4 - Parse a config.yaml */

  // See
  // https://github.com/open-telemetry/opentelemetry-configuration/blob/main/examples/kitchen-sink.yaml
  auto model = opentelemetry::sdk::configuration::YamlConfigurationFactory::ParseFile(config_file);

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

void usage(FILE *out)
{
  static const char *msg =
      "Usage: example_yaml [options]\n"
      "Valid options are:\n"
      "  --help            Print this help\n"
      "  --yaml            Path to a yaml configuration file\n"
      "\n"
      "The configuration file used will be:\n"
      "  1) the file provided in the command line\n"
      "  2) the file provided in environment variable ${OTEL_CONFIG_FILE}\n"
      "  3) file config.yaml\n";

  fprintf(out, "%s", msg);
}

int parse_args(int argc, char *argv[])
{
  int remaining_argc    = argc;
  char **remaining_argv = argv;

  while (remaining_argc > 0)
  {
    if (strcmp(*remaining_argv, "--help") == 0)
    {
      opt_help = true;
      return 0;
    }

    if (remaining_argc >= 2)
    {
      if (strcmp(*remaining_argv, "--yaml") == 0)
      {
        remaining_argc--;
        remaining_argv++;
        yaml_file_path = *remaining_argv;
        remaining_argc--;
        remaining_argv++;
        continue;
      }
    }

    if (remaining_argc)
    {
      // Unknown option
      return 1;
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  // Program name
  argc--;
  argv++;

  int rc = parse_args(argc, argv);

  if (rc != 0)
  {
    usage(stderr);
    return 1;
  }

  if (opt_help)
  {
    usage(stdout);
    return 0;
  }

  InitOtel(yaml_file_path);

  foo_library();

  CleanupOtel();
}
