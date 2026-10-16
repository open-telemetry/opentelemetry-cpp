// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/exporters/otlp/otlp_json_writer_factory_nlohmann.h"
#include "opentelemetry/version.h"
#include "otlp_json_writer_contract_test.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace test
{

INSTANTIATE_TEST_SUITE_P(Nlohmann,
                         JsonWriterContract,
                         ::testing::Values(std::make_shared<JsonWriterFactoryNlohmann>()));

TEST(NlohmannJsonWriter, SortsMembersByKey)
{
  auto writer = JsonWriterFactoryNlohmann().Create();
  writer->BeginObject();
  writer->Key("b");
  writer->BeginArray();
  writer->WriteInt32(1);
  writer->WriteInt32(2);
  writer->EndArray();
  writer->Key("a");
  writer->WriteString("one");
  writer->EndObject();
  EXPECT_EQ(writer->ToString(), R"({"a":"one","b":[1,2]})");
  EXPECT_TRUE(writer->ok());
}

}  // namespace test
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
