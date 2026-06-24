// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/version.h"  // IWYU pragma: keep

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

#  include <gtest/gtest.h>
#  include <cstddef>
#  include <memory>
#  include <stdexcept>
#  include <string>
#  include <utility>
#  include <vector>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/nostd/function_ref.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/semconv/exception_attributes.h"
#  include "opentelemetry/trace/noop.h"
#  include "opentelemetry/trace/span.h"
#  include "opentelemetry/trace/span_context.h"
#  include "opentelemetry/trace/tracer.h"

namespace trace_api = opentelemetry::trace;
namespace common    = opentelemetry::common;
namespace nostd     = opentelemetry::nostd;
namespace semconv   = opentelemetry::semconv;

namespace
{

class RecordingSpan final : public trace_api::Span
{
public:
  struct Event
  {
    std::string name;
    std::vector<std::pair<std::string, std::string>> attributes;
    size_t reported_size = 0;  // value returned by ExceptionAttributes::size()
  };

  void SetAttribute(nostd::string_view /*key*/,
                    const common::AttributeValue & /*value*/) noexcept override
  {}

  void AddEvent(nostd::string_view /*name*/) noexcept override {}
  void AddEvent(nostd::string_view /*name*/,
                common::SystemTimestamp /*timestamp*/) noexcept override
  {}

  void AddEvent(nostd::string_view name,
                common::SystemTimestamp /*timestamp*/,
                const common::KeyValueIterable &attributes) noexcept override
  {
    Event event;
    event.name          = std::string{name};
    event.reported_size = attributes.size();
    attributes.ForEachKeyValue([&event](nostd::string_view key, common::AttributeValue value) {
      if (nostd::holds_alternative<nostd::string_view>(value))
      {
        event.attributes.emplace_back(std::string{key},
                                      std::string{nostd::get<nostd::string_view>(value)});
      }
      else if (nostd::holds_alternative<const char *>(value))
      {
        event.attributes.emplace_back(std::string{key}, nostd::get<const char *>(value));
      }
      return true;
    });
    events_.push_back(std::move(event));
  }

  void AddLink(const trace_api::SpanContext & /*target*/,
               const common::KeyValueIterable & /*attrs*/) noexcept override
  {}
  void AddLinks(const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept override {}

  void SetStatus(trace_api::StatusCode /*code*/,
                 nostd::string_view /*description*/) noexcept override
  {}
  void UpdateName(nostd::string_view /*name*/) noexcept override {}
  void End(const trace_api::EndSpanOptions & /*options*/) noexcept override {}
  trace_api::SpanContext GetContext() const noexcept override
  {
    return trace_api::SpanContext(false, false);
  }
  bool IsRecording() const noexcept override { return true; }

  const std::vector<Event> &events() const noexcept { return events_; }

private:
  std::vector<Event> events_;
};

class TestException : public std::runtime_error
{
public:
  TestException() : std::runtime_error("something failed") {}
};

int CountAttribute(const RecordingSpan::Event &event,
                   nostd::string_view key,
                   std::string *value = nullptr)
{
  int count = 0;
  for (const auto &kv : event.attributes)
  {
    if (nostd::string_view{kv.first} == key)
    {
      ++count;
      if (value != nullptr)
      {
        *value = kv.second;
      }
    }
  }
  return count;
}

}  // namespace

TEST(RecordExceptionTest, SetsMessageFromWhat)
{
  RecordingSpan span;
  TestException ex;
  span.RecordException(ex);

  ASSERT_EQ(span.events().size(), 1u);
  const auto &event = span.events().front();
  EXPECT_EQ(event.name, "exception");

  std::string message;
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionMessage, &message), 1);
  EXPECT_EQ(message, "something failed");
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionType), 0);
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionStacktrace), 0);

  EXPECT_EQ(event.reported_size, event.attributes.size());
}

TEST(RecordExceptionTest, ForwardsCallerAttributes)
{
  RecordingSpan span;
  TestException ex;
  span.RecordException(ex, {{semconv::exception::kExceptionType, "TestException"},
                            {semconv::exception::kExceptionStacktrace, "frame0\nframe1"}});

  ASSERT_EQ(span.events().size(), 1u);
  const auto &event = span.events().front();
  EXPECT_EQ(event.name, "exception");

  std::string value;
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionMessage, &value), 1);
  EXPECT_EQ(value, "something failed");
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionType, &value), 1);
  EXPECT_EQ(value, "TestException");
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionStacktrace, &value), 1);
  EXPECT_EQ(value, "frame0\nframe1");

  EXPECT_EQ(event.reported_size, event.attributes.size());
}

TEST(RecordExceptionTest, CallerMessageTakesPrecedence)
{
  RecordingSpan span;
  TestException ex;
  span.RecordException(ex, {{semconv::exception::kExceptionMessage, "overridden"}});

  ASSERT_EQ(span.events().size(), 1u);
  const auto &event = span.events().front();

  std::string message;
  EXPECT_EQ(CountAttribute(event, semconv::exception::kExceptionMessage, &message), 1);
  EXPECT_EQ(message, "overridden");

  EXPECT_EQ(event.reported_size, event.attributes.size());
}

TEST(RecordExceptionTest, NoopSpanDoesNotRecord)
{
  std::shared_ptr<trace_api::Tracer> tracer{new trace_api::NoopTracer{}};
  auto span = tracer->StartSpan("operation");
  TestException ex;
  span->RecordException(ex);
  span->RecordException(ex, {{semconv::exception::kExceptionType, "TestException"}});
  EXPECT_FALSE(span->IsRecording());
}

#endif /* OPENTELEMETRY_ABI_VERSION_NO */
