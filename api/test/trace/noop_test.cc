#include "opentelemetry/trace/noop.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::core::SystemTimestamp;
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
  SpanContext sp(false, false);
  s1->AddLink(sp);
  using M = std::map<std::string, std::string>;
  M m1    = {{"abc", "123"}, {"xyz", "456"}};
  opentelemetry::trace::KeyValueIterableView<M> iterable{m1};

  s1->AddLink(sp, iterable);
  s1->AddLink(sp, {{"abc", "123"}, {"xyz", "456"}});

  EXPECT_EQ(s1->IsRecording(), false);

  s1->SetStatus(opentelemetry::trace::CanonicalCode::INVALID_ARGUMENT, "span unavailable");

  s1->UpdateName("test_name");

  SystemTimestamp t1;
  s1->AddEvent("test_time_stamp", t1);

  s1->GetContext();
}
