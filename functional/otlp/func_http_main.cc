// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include <string>

namespace trace     = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace otlp      = opentelemetry::exporter::otlp;
namespace nostd     = opentelemetry::nostd;

namespace internal_log = opentelemetry::sdk::common::internal_log;

bool opt_help  = false;
bool opt_list  = false;
bool opt_debug = false;
// HTTPS by default
std::string opt_endpoint = "https://localhost:4318/v1/traces";
std::string opt_cert_dir;
std::string opt_test_name;

void init(const otlp::OtlpHttpExporterOptions &opts)
{
  // Create OTLP exporter instance
  auto exporter  = otlp::OtlpHttpExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}

void payload()
{
  static const nostd::string_view k_tracer_name("func_test");
  static const nostd::string_view k_span_name("func_http_main");
  static const nostd::string_view k_attr_test_name("test_name");

  auto provider = trace::Provider::GetTracerProvider();
  auto tracer   = provider->GetTracer(k_tracer_name, "1.0");

  auto span = tracer->StartSpan(k_span_name);
  span->SetAttribute(k_attr_test_name, opt_test_name);
  span->End();
}

void cleanup()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace::Provider::SetTracerProvider(none);
}

void instrumented_payload(const otlp::OtlpHttpExporterOptions &opts)
{
  init(opts);
  payload();
  cleanup();
}

void usage(FILE *out)
{
  static const char *msg =
      "Usage: func_otlp_http [options] test_name\n"
      "Valid options are:\n"
      "  --help            Print this help\n"
      "  --list            List test names\n"
      "  --endpoint url    OTLP HTTP endpoint (https://localhost:4318/v1/traces by default)\n"
      "  --cert-dir dir    Directory that contains test ssl certificates\n";
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

    if (strcmp(*remaining_argv, "--list") == 0)
    {
      opt_list = true;
      return 0;
    }

    if (strcmp(*remaining_argv, "--debug") == 0)
    {
      opt_debug = true;
      remaining_argc--;
      remaining_argv++;
      continue;
    }

    if (remaining_argc >= 2)
    {
      if (strcmp(*remaining_argv, "--cert-dir") == 0)
      {
        remaining_argc--;
        remaining_argv++;
        opt_cert_dir = *remaining_argv;
        remaining_argc--;
        remaining_argv++;
        continue;
      }

      if (strcmp(*remaining_argv, "--endpoint") == 0)
      {
        remaining_argc--;
        remaining_argv++;
        opt_endpoint = *remaining_argv;
        remaining_argc--;
        remaining_argv++;
        continue;
      }
    }

    opt_test_name = *remaining_argv;
    remaining_argc--;
    remaining_argv++;

    if (remaining_argc)
    {
      // Unknown option
      return 1;
    }
  }

  return 0;
}

typedef int (*test_func_t)();

struct test_case
{
  std::string m_name;
  test_func_t m_func;
};

int test_basic();

#ifdef ENABLE_OTLP_HTTP_SSL
int test_cert_not_found();
int test_cert_invalid();
int test_cert_unreadable();
int test_cert_ok();
int test_client_cert_not_found();
int test_client_cert_invalid();
int test_client_cert_unreadable();
int test_client_cert_no_key();
int test_client_key_not_found();
int test_client_key_invalid();
int test_client_key_unreadable();
int test_client_key_ok();
#endif /* ENABLE_OTLP_HTTP_SSL */

#ifdef ENABLE_OTLP_HTTP_SSL_TLS
int test_min_tls_unknown();
int test_min_tls_10();
int test_min_tls_11();
int test_min_tls_12();
int test_min_tls_13();
int test_max_tls_unknown();
int test_max_tls_10();
int test_max_tls_11();
int test_max_tls_12();
int test_max_tls_13();
int test_range_tls_10();
int test_range_tls_11();
int test_range_tls_12();
int test_range_tls_13();
int test_range_tls_10_11();
int test_range_tls_10_12();
int test_range_tls_10_13();
int test_range_tls_11_10();
int test_range_tls_11_12();
int test_range_tls_11_13();
int test_range_tls_12_10();
int test_range_tls_12_11();
int test_range_tls_12_13();
int test_range_tls_13_10();
int test_range_tls_13_11();
int test_range_tls_13_12();
#endif /* ENABLE_OTLP_HTTP_SSL_TLS */

static const test_case all_tests[] = {{"basic", test_basic},
#ifdef ENABLE_OTLP_HTTP_SSL
                                      {"cert-not-found", test_cert_not_found},
                                      {"cert-invalid", test_cert_invalid},
                                      {"cert-unreadable", test_cert_unreadable},
                                      {"cert-ok", test_cert_ok},
                                      {"client-cert-not-found", test_client_cert_not_found},
                                      {"client-cert-invalid", test_client_cert_invalid},
                                      {"client-cert-unreadable", test_client_cert_unreadable},
                                      {"client-cert-no-key", test_client_cert_no_key},
                                      {"client-key-not-found", test_client_key_not_found},
                                      {"client-key-invalid", test_client_key_invalid},
                                      {"client-key-unreadable", test_client_key_unreadable},
                                      {"client-key-ok", test_client_key_ok},
#endif /* ENABLE_OTLP_HTTP_SSL */

#ifdef ENABLE_OTLP_HTTP_SSL_TLS
                                      {"min-tls-unknown", test_min_tls_unknown},
                                      {"min-tls-10", test_min_tls_10},
                                      {"min-tls-11", test_min_tls_11},
                                      {"min-tls-12", test_min_tls_12},
                                      {"min-tls-13", test_min_tls_13},
                                      {"max-tls-unknown", test_max_tls_unknown},
                                      {"max-tls-10", test_max_tls_10},
                                      {"max-tls-11", test_max_tls_11},
                                      {"max-tls-12", test_max_tls_12},
                                      {"max-tls-13", test_max_tls_13},
                                      {"range-tls-10", test_range_tls_10},
                                      {"range-tls-11", test_range_tls_11},
                                      {"range-tls-12", test_range_tls_12},
                                      {"range-tls-13", test_range_tls_13},
                                      {"range-tls-10-11", test_range_tls_10_11},
                                      {"range-tls-10-12", test_range_tls_10_12},
                                      {"range-tls-10-13", test_range_tls_10_13},
                                      {"range-tls-11-10", test_range_tls_11_10},
                                      {"range-tls-11-12", test_range_tls_11_12},
                                      {"range-tls-11-13", test_range_tls_11_13},
                                      {"range-tls-12-10", test_range_tls_12_10},
                                      {"range-tls-12-11", test_range_tls_12_11},
                                      {"range-tls-12-13", test_range_tls_12_13},
                                      {"range-tls-13-10", test_range_tls_13_10},
                                      {"range-tls-13-11", test_range_tls_13_11},
                                      {"range-tls-13-12", test_range_tls_13_12},
#endif /* ENABLE_OTLP_HTTP_SSL_TLS */
                                      {"", nullptr}};

void list_test_cases()
{
  const test_case *current = all_tests;

  while (current->m_func != nullptr)
  {
    std::cout << current->m_name << std::endl;
    current++;
  }
}

int run_test_case(std::string name)
{
  const test_case *current = all_tests;

  while (current->m_func != nullptr)
  {
    if (current->m_name == name)
    {
      int rc = current->m_func();
      return rc;
    }
    current++;
  }

  std::cerr << "Unknown test <" << name << ">" << std::endl;
  return 1;
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

  if (opt_list)
  {
    list_test_cases();
    return 0;
  }

  rc = run_test_case(opt_test_name);
  return rc;
}

void set_common_opts(otlp::OtlpHttpExporterOptions &opts)
{
  opts.url = opt_endpoint;
}

int test_basic()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);

  instrumented_payload(opts);
  return 0;
}

#ifdef ENABLE_OTLP_HTTP_SSL

int test_cert_not_found()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path = opt_cert_dir + "/no-such-file.pem";

  instrumented_payload(opts);
  return 0;
}

int test_cert_invalid()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path = opt_cert_dir + "/garbage.pem";

  instrumented_payload(opts);
  return 0;
}

int test_cert_unreadable()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  // FIXME: file unreadable.pem exists locally with chmod 0,
  // but then it can not be added in git.
  opts.ssl_ca_cert_path = opt_cert_dir + "/unreadable.pem";

  instrumented_payload(opts);
  return 0;
}

int test_cert_ok()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path = opt_cert_dir + "/ca.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_cert_not_found()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/no-such-file.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_cert_invalid()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/garbage.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_cert_unreadable()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/unreadable.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_cert_no_key()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_key_not_found()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/no-such-file.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_key_invalid()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/garbage.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_key_unreadable()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/unreadable.pem";

  instrumented_payload(opts);
  return 0;
}

int test_client_key_ok()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";

  instrumented_payload(opts);
  return 0;
}
#endif /* ENABLE_OTLP_HTTP_SSL */

#ifdef ENABLE_OTLP_HTTP_SSL_TLS

int test_min_tls_unknown()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "99.99";

  instrumented_payload(opts);
  return 0;
}

int test_min_tls_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_min_tls_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_min_tls_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

int test_min_tls_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_max_tls_unknown()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_max_tls          = "99.99";

  instrumented_payload(opts);
  return 0;
}

int test_max_tls_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_max_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_max_tls_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_max_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_max_tls_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_max_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

int test_max_tls_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_max_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.0";
  opts.ssl_max_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.1";
  opts.ssl_max_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.2";
  opts.ssl_max_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.3";
  opts.ssl_max_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_10_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.0";
  opts.ssl_max_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_10_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.0";
  opts.ssl_max_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_10_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.0";
  opts.ssl_max_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_11_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.1";
  opts.ssl_max_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_11_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.1";
  opts.ssl_max_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_11_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.1";
  opts.ssl_max_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_12_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.2";
  opts.ssl_max_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_12_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.2";
  opts.ssl_max_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_12_13()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.2";
  opts.ssl_max_tls          = "1.3";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_13_10()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.3";
  opts.ssl_max_tls          = "1.0";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_13_11()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.3";
  opts.ssl_max_tls          = "1.1";

  instrumented_payload(opts);
  return 0;
}

int test_range_tls_13_12()
{
  otlp::OtlpHttpExporterOptions opts;

  set_common_opts(opts);
  opts.ssl_ca_cert_path     = opt_cert_dir + "/ca.pem";
  opts.ssl_client_cert_path = opt_cert_dir + "/client_cert.pem";
  opts.ssl_client_key_path  = opt_cert_dir + "/client_cert-key.pem";
  opts.ssl_min_tls          = "1.3";
  opts.ssl_max_tls          = "1.2";

  instrumented_payload(opts);
  return 0;
}

#endif /* ENABLE_OTLP_HTTP_SSL_TLS */
