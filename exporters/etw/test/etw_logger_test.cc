// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef _WIN32

#  include <gtest/gtest.h>
#  include <map>
#  include <string>

#  include "opentelemetry/exporters/etw/etw_logger_exporter.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"

using namespace OPENTELEMETRY_NAMESPACE;

using namespace opentelemetry::exporter::etw;

const char *kGlobalProviderName = "OpenTelemetry-ETW-TLD";

/**
 * @brief Logger test with name and unstructured text
 * {
 * "Timestamp": "2021-09-30T16:40:40.0820563-07:00",
 * "ProviderName": "OpenTelemetry-ETW-TLD",
 * "Id": 2,
 * "Message": null,
 * "ProcessId": 23180,
 * "Level": "Always",
 * "Keywords": "0x0000000000000000",
 * "EventName": "Log",
 * "ActivityID": null,
 * "RelatedActivityID": null,
 * "Payload": {
 *   "Name": "test",
 *   "SpanId": "0000000000000000",
 *   "Timestamp": "2021-09-30T23:40:40.066411500Z",
 *   "TraceId": "00000000000000000000000000000000",
 *   "_name": "Log",
 *   "body": "This is test message",
 *   "severityNumber": 5,
 *   "severityText": "DEBUG"
 * }
 * }
 */

TEST(ETWLogger, LoggerCheckWithBody)
{
  std::string providerName = kGlobalProviderName;  // supply unique instrumentation name here
  exporter::etw::LoggerProvider lp;

  const std::string schema_url{"https://opentelemetry.io/schemas/1.2.0"};
  auto logger        = lp.GetLogger(providerName, schema_url);
  Properties attribs = {{"attrib1", 1}, {"attrib2", 2}};
  EXPECT_NO_THROW(
      logger->EmitLogRecord(opentelemetry::logs::Severity::kDebug, "This is test log body"));
}

/**
 * @brief Logger Test with structured attributes
 *
 * Example Event for below test:
 * {
 *  "Timestamp": "2021-09-30T15:04:15.4227815-07:00",
 *  "ProviderName": "OpenTelemetry-ETW-TLD",
 * "Id": 1,
 * "Message": null,
 * "ProcessId": 33544,
 * "Level": "Always",
 * "Keywords": "0x0000000000000000",
 * "EventName": "Log",
 * "ActivityID": null,
 * "RelatedActivityID": null,
 * "Payload": {
 *  "Name": "test",
 *   "SpanId": "0000000000000000",
 *  "Timestamp": "2021-09-30T22:04:15.066411500Z",
 *   "TraceId": "00000000000000000000000000000000",
 *   "_name": "Log",
 *  "attrib1": 1,
 *   "attrib2": 2,
 *   "body": "",
 *   "severityNumber": 5,
 *   "severityText": "DEBUG"
 * }
 * }
 *
 */

TEST(ETWLogger, LoggerCheckWithAttributes)
{
  std::string providerName = kGlobalProviderName;  // supply unique instrumentation name here
  exporter::etw::LoggerProvider lp;

  const std::string schema_url{"https://opentelemetry.io/schemas/1.2.0"};
  auto logger = lp.GetLogger(providerName, schema_url);
  // Log attributes
  Properties attribs = {{"attrib1", 1}, {"attrib2", 2}};
  EXPECT_NO_THROW(logger->EmitLogRecord(opentelemetry::logs::Severity::kDebug,
                                        opentelemetry::common::MakeAttributes(attribs)));
}

#endif  // _WIN32
