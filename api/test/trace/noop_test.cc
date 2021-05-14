// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/trace/noop.h"
#include "opentelemetry/common/timestamp.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::common::SystemTimestamp;
using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::SpanContext;
using opentelemetry::trace::Tracer;

TEST(NoopTest, UseNoopTracers)
{
  std::shared_ptr<Tracer> tracer{new NoopTracer{}};
  auto s1 = tracer->StartSpan("abc");

  std::map<std::string, std::string> attributes1;
  s1->AddEvent("abc", attributes1);

  std::vector<std::pair<std::string, int>> attributes2;
  s1->AddEvent("abc", attributes2);

  s1->AddEvent("abc", {{"a", 1}, {"b", "2"}, {"c", 3.0}});

  std::vector<std::pair<std::string, std::vector<int>>> attributes3;
  s1->AddEvent("abc", attributes3);

  s1->SetAttribute("abc", 4);

  s1->AddEvent("abc");  // add Empty

  EXPECT_EQ(s1->IsRecording(), false);

  s1->SetStatus(opentelemetry::trace::StatusCode::kUnset, "span unset");

  s1->UpdateName("test_name");

  SystemTimestamp t1;
  s1->AddEvent("test_time_stamp", t1);

  s1->GetContext();
}

TEST(NoopTest, StartSpan)
{
  std::shared_ptr<Tracer> tracer{new NoopTracer{}};

  std::map<std::string, std::string> attrs                                      = {{"a", "3"}};
  std::vector<std::pair<SpanContext, std::map<std::string, std::string>>> links = {
      {SpanContext(false, false), attrs}};
  auto s1 = tracer->StartSpan("abc", attrs, links);

  auto s2 = tracer->StartSpan("efg", {{"a", 3}}, {{SpanContext(false, false), {{"b", 4}}}});
}
