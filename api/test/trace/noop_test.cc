#include "opentelemetry/trace/noop.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::Tracer;

TEST(NoopTest, UseNoopTracers)
{
  //  Note: This test is no longer valid as Span no longer has field of tracer. Whether
  //  removing it depends on the creator of this file.
  //  std::shared_ptr<Tracer> tracer{new NoopTracer{}};
  //  auto s1 = tracer->StartSpan("abc");
  //  EXPECT_EQ(&s1->tracer(), tracer.get());
  //
  //  std::map<std::string, std::string> attributes1;
  //  s1->AddEvent("abc", attributes1);
  //
  //  std::vector<std::pair<std::string, int>> attributes2;
  //  s1->AddEvent("abc", attributes2);
  //
  //  s1->AddEvent("abc", {{"a", 1}, {"b", "2"}, {"c", 3.0}});
  //
  //  std::vector<std::pair<std::string, std::vector<int>>> attributes3;
  //  s1->AddEvent("abc", attributes3);
  //
  //  s1->SetAttribute("abc", 4);
}
