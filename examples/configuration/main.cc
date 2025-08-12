// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "opentelemetry/exporters/ostream/console_log_record_builder.h"
#include "opentelemetry/exporters/ostream/console_push_metric_builder.h"
#include "opentelemetry/exporters/ostream/console_span_builder.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/configuration.h"
#include "opentelemetry/sdk/configuration/configured_sdk.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/yaml_configuration_parser.h"

#include "custom_log_record_exporter_builder.h"
#include "custom_log_record_processor_builder.h"
#include "custom_pull_metric_exporter_builder.h"
#include "custom_push_metric_exporter_builder.h"
#include "custom_sampler_builder.h"
#include "custom_span_exporter_builder.h"
#include "custom_span_processor_builder.h"

#ifdef BAZEL_BUILD
#  include "examples/common/logs_foo_library/foo_library.h"
#  include "examples/common/metrics_foo_library/foo_library.h"
#else
#  include "logs_foo_library/foo_library.h"
#  include "metrics_foo_library/foo_library.h"
#endif

#ifdef OTEL_HAVE_OTLP_HTTP
#  include "opentelemetry/exporters/otlp/otlp_http_log_record_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_http_push_metric_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_http_span_builder.h"
#endif

#ifdef OTEL_HAVE_OTLP_GRPC
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_record_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_push_metric_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_span_builder.h"
#endif

#ifdef OTEL_HAVE_OTLP_FILE
#  include "opentelemetry/exporters/otlp/otlp_file_log_record_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_file_push_metric_builder.h"
#  include "opentelemetry/exporters/otlp/otlp_file_span_builder.h"
#endif

#ifdef OTEL_HAVE_ZIPKIN
#  include "opentelemetry/exporters/zipkin/zipkin_builder.h"
#endif

#ifdef OTEL_HAVE_PROMETHEUS
#  include "opentelemetry/exporters/prometheus/prometheus_pull_builder.h"
#endif

static bool opt_help              = false;
static bool opt_debug             = false;
static bool opt_test              = false;
static bool opt_no_registry       = false;
static std::string yaml_file_path = "";

static std::unique_ptr<opentelemetry::sdk::configuration::ConfiguredSdk> sdk;

namespace
{

class CheckerLogHandler : public opentelemetry::sdk::common::internal_log::LogHandler
{
public:
  void Handle(opentelemetry::sdk::common::internal_log::LogLevel level,
              const char * /* file */,
              int /* line */,
              const char *msg,
              const opentelemetry::sdk::common::AttributeMap & /* attributes */) noexcept override
  {
    if (msg == nullptr)
    {
      msg = "<no msg>";
    }
    switch (level)
    {
      case opentelemetry::sdk::common::internal_log::LogLevel::None:
        break;
      case opentelemetry::sdk::common::internal_log::LogLevel::Error:
        fprintf(stdout, "[ERROR] %s\n", msg);
        break;
      case opentelemetry::sdk::common::internal_log::LogLevel::Warning:
        fprintf(stdout, "[WARNING] %s\n", msg);
        break;
      case opentelemetry::sdk::common::internal_log::LogLevel::Info:
        fprintf(stdout, "[INFO] %s\n", msg);
        break;
      case opentelemetry::sdk::common::internal_log::LogLevel::Debug:
        fprintf(stdout, "[DEBUG] %s\n", msg);
        break;
    }
  }
};

void SetLoggerForTesting()
{
  if (opt_test)
  {
    opentelemetry::nostd::shared_ptr<opentelemetry::sdk::common::internal_log::LogHandler>
        checker_log_handler(new CheckerLogHandler());

    opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler(checker_log_handler);
  }
}

void PrintModelParsedForTesting(bool pass)
{
  if (opt_test)
  {
    if (pass)
    {
      fprintf(stdout, "MODEL PARSED\n");
    }
    else
    {
      fprintf(stdout, "FAILED TO PARSE MODEL\n");
      exit(1);
    }
  }
}

void PrintSdkCreatedForTesting(bool pass)
{
  if (opt_test)
  {
    if (pass)
    {
      fprintf(stdout, "SDK CREATED\n");
    }
    else
    {
      fprintf(stdout, "FAILED TO CREATE SDK\n");
      exit(2);
    }
  }
}

void SetSilentLoggerForTesting()
{
  if (opt_test)
  {
    // Do not record noise during payload,
    // so it does not go into the shelltest output.
    auto level = opentelemetry::sdk::common::internal_log::LogLevel::None;
    opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(level);
  }
}

void InitOtel(const std::string &config_file)
{
  auto level = opentelemetry::sdk::common::internal_log::LogLevel::Info;

  if (opt_debug)
  {
    level = opentelemetry::sdk::common::internal_log::LogLevel::Debug;
  }

  opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(level);

  /* 1 - Create a registry */

  std::shared_ptr<opentelemetry::sdk::configuration::Registry> registry(
      new opentelemetry::sdk::configuration::Registry);

  /* 2 - Populate the registry with the core components supported */

  /*
   * Note:
   *
   * In a typical application, decide which component are allowed, and just
   * Register them, without compile time (#ifdef OTEL_HAVE_OTLP_HTTP)
   * or runtime (opt_no_registry) logic.
   *
   * Because this example is used in CI with various build configurations,
   * we add compile time checks to have a working build in all cases.
   * Do not use ifdef, just pick the components the application uses.
   *
   * Also, because this example is used in CI to run functional tests,
   * we add a runtime check to execute stress tests for coverage.
   * Do not blindly copy and paste if(!opt_no_registry), it is test code.
   */

  if (!opt_no_registry)
  {
    opentelemetry::exporter::trace::ConsoleSpanBuilder::Register(registry.get());
    opentelemetry::exporter::metrics::ConsolePushMetricBuilder::Register(registry.get());
    opentelemetry::exporter::logs::ConsoleLogRecordBuilder::Register(registry.get());

#ifdef OTEL_HAVE_OTLP_HTTP
    opentelemetry::exporter::otlp::OtlpHttpSpanBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpHttpPushMetricBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpHttpLogRecordBuilder::Register(registry.get());
#endif

#ifdef OTEL_HAVE_OTLP_GRPC
    opentelemetry::exporter::otlp::OtlpGrpcSpanBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpGrpcPushMetricBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpGrpcLogRecordBuilder::Register(registry.get());
#endif

#ifdef OTEL_HAVE_OTLP_FILE
    opentelemetry::exporter::otlp::OtlpFileSpanBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpFilePushMetricBuilder::Register(registry.get());
    opentelemetry::exporter::otlp::OtlpFileLogRecordBuilder::Register(registry.get());
#endif

#ifdef OTEL_HAVE_ZIPKIN
    opentelemetry::exporter::zipkin::ZipkinBuilder::Register(registry.get());
#endif

#ifdef OTEL_HAVE_PROMETHEUS
    opentelemetry::exporter::metrics::PrometheusPullBuilder::Register(registry.get());
#endif
  }

  /* 3 - Populate the registry with external extensions plugins */

  CustomSamplerBuilder::Register(registry.get());
  CustomSpanExporterBuilder::Register(registry.get());
  CustomSpanProcessorBuilder::Register(registry.get());
  CustomPushMetricExporterBuilder::Register(registry.get());
  CustomPullMetricExporterBuilder::Register(registry.get());
  CustomLogRecordExporterBuilder::Register(registry.get());
  CustomLogRecordProcessorBuilder::Register(registry.get());

  /* 4 - Parse a config.yaml */

  // See
  // https://github.com/open-telemetry/opentelemetry-configuration/blob/main/examples/kitchen-sink.yaml
  auto model = opentelemetry::sdk::configuration::YamlConfigurationParser::ParseFile(config_file);

  // Functional test helpers, ignore.
  PrintModelParsedForTesting(model != nullptr);

  /* 5 - Build the SDK from the parsed config.yaml */

  sdk = opentelemetry::sdk::configuration::ConfiguredSdk::Create(registry, model);

  // Functional test helpers, ignore.
  PrintSdkCreatedForTesting(sdk != nullptr);

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

static void usage(FILE *out)
{
  static const char *msg =
      "Usage: example_yaml [options]\n"
      "Valid options are:\n"
      "  --help\n"
      "    Print this help\n"
      "  --yaml </path/to/config.yaml>\n"
      "    Path to a yaml configuration file\n"
      "  --debug\n"
      "    Set logger to debug\n"
      "\n"
      "The configuration file used will be:\n"
      "  1) the file provided in the command line\n"
      "  2) the file provided in environment variable ${OTEL_EXPERIMENTAL_CONFIG_FILE}\n"
      "  3) file config.yaml\n"
      "\n"
      "This utility is also used for functional tests.\n"
      "\n"
      "Valid test options are:\n"
      "  --test\n"
      "    Run in test mode\n"
      "  --no-registry\n"
      "    Run with an empty registry\n";

  fprintf(out, "%s", msg);
}

static int parse_args(int argc, char *argv[])
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

    if (strcmp(*remaining_argv, "--debug") == 0)
    {
      remaining_argc--;
      remaining_argv++;
      opt_debug = true;
      continue;
    }

    if (strcmp(*remaining_argv, "--test") == 0)
    {
      remaining_argc--;
      remaining_argv++;
      opt_test = true;
      continue;
    }

    if (strcmp(*remaining_argv, "--no-registry") == 0)
    {
      remaining_argc--;
      remaining_argv++;
      opt_no_registry = true;
      continue;
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

  // Functional test helpers, ignore.
  SetLoggerForTesting();

  InitOtel(yaml_file_path);

  // Functional test helpers, ignore.
  SetSilentLoggerForTesting();

  foo_library();
  foo_library::counter_example("yaml");
  foo_library::observable_counter_example("yaml");
  foo_library::histogram_example("yaml");

  CleanupOtel();
  return 0;
}
