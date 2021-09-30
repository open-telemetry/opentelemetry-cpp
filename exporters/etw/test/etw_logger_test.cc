// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  ifdef _WIN32

#    include <gtest/gtest.h>
#    include <map>
#    include <string>

#    include "opentelemetry/exporters/etw/etw_logger.h"
#    include "opentelemetry/sdk/trace/simple_processor.h"

using namespace OPENTELEMETRY_NAMESPACE;

using namespace opentelemetry::exporter::etw;

const char *kGlobalProviderName = "OpenTelemetry-ETW-TLD";

TEST(ETWTracer, LoggerCheck)
{
  std::string providerName = kGlobalProviderName;  // supply unique instrumentation name here
  exporter::etw::LoggerProvider lp;

  auto logger = lp.GetLogger(providerName);
  // Span attributes
  Properties attribs = {{"attrib1", 1}, {"attrib2", 2}};
  logger->Log(opentelemetry::logs::Severity::kDebug, "test", attribs);
}

#  endif  // _WIN32
#endif    // ENABLE_LOGS_PREVIEW
