// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace test
{

// Tests a JsonWriter backend against the contract documented in otlp_json_writer.h. The test of a
// backend compiles otlp_json_writer_contract_test.cc and instantiates the suite with its factory:
//
//   INSTANTIATE_TEST_SUITE_P(Name, JsonWriterContract,
//                            ::testing::Values(std::make_shared<NameJsonWriterFactory>()));
class JsonWriterContract : public ::testing::TestWithParam<std::shared_ptr<JsonWriterFactory>>
{
protected:
  std::unique_ptr<JsonWriter> MakeWriter() const;

  // Expects the writer to have accepted a well-formed stream without logging an error, and returns
  // its document.
  std::string Finish(JsonWriter &writer);

  // Expects the writer to have failed on a malformed stream, to stay failed through more tokens
  // and ToString(), and to have logged the failure once.
  void ExpectFailedOnce(JsonWriter &writer);

private:
  test_common::ScopedTestLogHandler log_{sdk::common::internal_log::LogLevel::Error};
};

}  // namespace test
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
