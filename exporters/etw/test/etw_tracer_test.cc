// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <string>

#define HAVE_NO_TLD

#include "opentelemetry/exporters/etw/etw_tracer_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"

using namespace OPENTELEMETRY_NAMESPACE;

using ETWEvent = std::map<nostd::string_view, common::AttributeValue>;

TEST(ETWTracer, TracerCheck)
{
  std::string providerName = "OpenTelemetry";
  std::string eventName    = "MyEvent";

  ETW::TracerProvider tp;
  auto tracer = tp.GetTracer(providerName);
  auto span   = tracer->StartSpan("MySpan");

  ETWEvent event = {
      {"uint32Key", (uint32_t)123456}, {"uint64Key", (uint64_t)123456}, {"strKey", "someValue"}};

  EXPECT_NO_THROW(span->AddEvent(eventName, event));
  EXPECT_NO_THROW(span->End());
  EXPECT_NO_THROW(tracer->Close());
}

TEST(ETWTracer, ETWTracerTest)
{
  std::string providerName = "OpenTelemetry";

  auto exporter = std::unique_ptr<ETW::ETWTracerExporter>(new ETW::ETWTracerExporter(providerName));

  auto processor = std::shared_ptr<sdk::trace::SpanProcessor>(new sdk::trace::SimpleSpanProcessor(std::move(exporter)));

  auto recordable = processor->MakeRecordable();
  recordable->SetName("MySpan");

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  processor->OnEnd(std::move(recordable));

  std::cout.rdbuf(sbuf);

  std::string expectedOutput = "MySpan\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(ETWTracer, ExportUnitTest)
{
  std::string providerName = "OpenTelemetry";

  auto exporter = std::unique_ptr<ETW::ETWTracerExporter>(new ETW::ETWTracerExporter(providerName));

  auto recordable = exporter->MakeRecordable();
  recordable->SetName("MySpan");

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(&recordable, 1);
  auto result = exporter->Export(batch);
  EXPECT_EQ(sdk::trace::ExportResult::kSuccess, result);
}
