/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "shim_utils.h"
#include "shim_mocks.h"

#include "opentracing/tracer.h"

#include <gtest/gtest.h>

namespace trace_api = opentelemetry::trace;
namespace baggage   = opentelemetry::baggage;
namespace common    = opentelemetry::common;
namespace nostd     = opentelemetry::nostd;
namespace shim      = opentelemetry::opentracingshim;

class ShimUtilsTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }
};

TEST_F(ShimUtilsTest, IsBaggageEmpty)
{
  auto none = nostd::shared_ptr<baggage::Baggage>(nullptr);
  ASSERT_TRUE(shim::utils::isBaggageEmpty(none));

  auto empty = nostd::shared_ptr<baggage::Baggage>(new baggage::Baggage({}));
  ASSERT_TRUE(shim::utils::isBaggageEmpty(empty));

  std::map<std::string, std::string> list{{ "foo", "bar" }};
  auto non_empty = nostd::shared_ptr<baggage::Baggage>(new baggage::Baggage(list));
  ASSERT_FALSE(shim::utils::isBaggageEmpty(non_empty));
}

TEST_F(ShimUtilsTest, StringFromValue)
{
  ASSERT_EQ(shim::utils::stringFromValue(true), "true");
  ASSERT_EQ(shim::utils::stringFromValue(false), "false");
  ASSERT_EQ(shim::utils::stringFromValue(1234.567890), "1234.567890");
  ASSERT_EQ(shim::utils::stringFromValue(42l), "42");
  ASSERT_EQ(shim::utils::stringFromValue(55ul), "55");
  ASSERT_EQ(shim::utils::stringFromValue(std::string{"a string"}), "a string");
  ASSERT_EQ(shim::utils::stringFromValue(opentracing::string_view{"a string view"}), "a string view");
  ASSERT_EQ(shim::utils::stringFromValue(nullptr), "");
  ASSERT_EQ(shim::utils::stringFromValue("a char ptr"), "a char ptr");

  opentracing::util::recursive_wrapper<opentracing::Values> values{};
  ASSERT_EQ(shim::utils::stringFromValue(values.get()), "");

  opentracing::util::recursive_wrapper<opentracing::Dictionary> dict{};
  ASSERT_EQ(shim::utils::stringFromValue(dict.get()), "");
}

TEST_F(ShimUtilsTest, AttributeFromValue)
{
  auto value = shim::utils::attributeFromValue(true);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeBool);
  ASSERT_TRUE(nostd::get<bool>(value));

  value = shim::utils::attributeFromValue(false);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeBool);
  ASSERT_FALSE(nostd::get<bool>(value));

  value = shim::utils::attributeFromValue(1234.567890);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeDouble);
  ASSERT_EQ(nostd::get<double>(value), 1234.567890);

  value = shim::utils::attributeFromValue(42l);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeInt64);
  ASSERT_EQ(nostd::get<int64_t>(value), 42l);

  value = shim::utils::attributeFromValue(55ul);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeUInt64);
  ASSERT_EQ(nostd::get<uint64_t>(value), 55ul);

  value = shim::utils::attributeFromValue(std::string{"a string"});
  ASSERT_EQ(value.index(), common::AttributeType::kTypeCString);
  ASSERT_STREQ(nostd::get<const char *>(value), "a string");

  value = shim::utils::attributeFromValue(opentracing::string_view{"a string view"});
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{"a string view"});

  value = shim::utils::attributeFromValue(nullptr);
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});

  value = shim::utils::attributeFromValue("a char ptr");
  ASSERT_EQ(value.index(), common::AttributeType::kTypeCString);
  ASSERT_STREQ(nostd::get<const char *>(value), "a char ptr");

  opentracing::util::recursive_wrapper<opentracing::Values> values{};
  value = shim::utils::attributeFromValue(values.get());
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});

  opentracing::util::recursive_wrapper<opentracing::Dictionary> dict{};
  value = shim::utils::attributeFromValue(dict.get());
  ASSERT_EQ(value.index(), common::AttributeType::kTypeString);
  ASSERT_EQ(nostd::get<nostd::string_view>(value), nostd::string_view{});
}

TEST_F(ShimUtilsTest, MakeOptionsShim_EmptyRefs)
{
  auto span_context_shim = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context = static_cast<opentracing::SpanContext*>(span_context_shim.get());
  
  opentracing::StartSpanOptions options;
  options.start_system_timestamp = opentracing::SystemTime::time_point::clock::now();
  options.start_steady_timestamp = opentracing::SteadyTime::time_point::clock::now();

  auto options_shim = shim::utils::makeOptionsShim(options);
  ASSERT_EQ(options_shim.start_system_time, common::SystemTimestamp{options.start_system_timestamp});
  ASSERT_EQ(options_shim.start_steady_time, common::SteadyTimestamp{options.start_steady_timestamp});
  ASSERT_EQ(nostd::get<trace_api::SpanContext>(options_shim.parent), trace_api::SpanContext::GetInvalid());
}

TEST_F(ShimUtilsTest, MakeOptionsShim_InvalidSpanContext)
{
  auto span_context_shim = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context = static_cast<opentracing::SpanContext*>(span_context_shim.get());
  
  opentracing::StartSpanOptions options;
  options.start_system_timestamp = opentracing::SystemTime::time_point::clock::now();
  options.start_steady_timestamp = opentracing::SteadyTime::time_point::clock::now();
  options.references = {{ opentracing::SpanReferenceType::FollowsFromRef, nullptr }};

  auto options_shim = shim::utils::makeOptionsShim(options);
  ASSERT_EQ(options_shim.start_system_time, common::SystemTimestamp{options.start_system_timestamp});
  ASSERT_EQ(options_shim.start_steady_time, common::SteadyTimestamp{options.start_steady_timestamp});
  ASSERT_EQ(nostd::get<trace_api::SpanContext>(options_shim.parent), trace_api::SpanContext::GetInvalid());
}

TEST_F(ShimUtilsTest, MakeOptionsShim_FirstChildOf)
{
  auto span_context_shim = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context = static_cast<opentracing::SpanContext*>(span_context_shim.get());
  
  opentracing::StartSpanOptions options;
  options.start_system_timestamp = opentracing::SystemTime::time_point::clock::now();
  options.start_steady_timestamp = opentracing::SteadyTime::time_point::clock::now();
  options.references = {
    { opentracing::SpanReferenceType::FollowsFromRef, nullptr },
    { opentracing::SpanReferenceType::ChildOfRef, span_context },
    { opentracing::SpanReferenceType::ChildOfRef, nullptr }
  };

  auto options_shim = shim::utils::makeOptionsShim(options);
  ASSERT_EQ(options_shim.start_system_time, common::SystemTimestamp{options.start_system_timestamp});
  ASSERT_EQ(options_shim.start_steady_time, common::SteadyTimestamp{options.start_steady_timestamp});
  ASSERT_EQ(nostd::get<trace_api::SpanContext>(options_shim.parent), span_context_shim->context());
}

TEST_F(ShimUtilsTest, MakeOptionsShim_FirstInList)
{
  auto span_context_shim = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context = static_cast<opentracing::SpanContext*>(span_context_shim.get());
  
  opentracing::StartSpanOptions options;
  options.start_system_timestamp = opentracing::SystemTime::time_point::clock::now();
  options.start_steady_timestamp = opentracing::SteadyTime::time_point::clock::now();
  options.references = {
    { opentracing::SpanReferenceType::FollowsFromRef, span_context },
    { opentracing::SpanReferenceType::FollowsFromRef, nullptr }
  };

  auto options_shim = shim::utils::makeOptionsShim(options);
  ASSERT_EQ(options_shim.start_system_time, common::SystemTimestamp{options.start_system_timestamp});
  ASSERT_EQ(options_shim.start_steady_time, common::SteadyTimestamp{options.start_steady_timestamp});
  ASSERT_EQ(nostd::get<trace_api::SpanContext>(options_shim.parent), span_context_shim->context());
}

TEST_F(ShimUtilsTest, MakeReferenceLinks)
{
  auto span_context_shim1 = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context1 = static_cast<opentracing::SpanContext*>(span_context_shim1.get());
  auto span_context_shim2 = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), baggage::Baggage::GetDefault()));
  auto span_context2 = static_cast<opentracing::SpanContext*>(span_context_shim2.get());

  opentracing::StartSpanOptions options;
  auto links = shim::utils::makeReferenceLinks(options);
  ASSERT_TRUE(links.empty());

  options.references = {
    { opentracing::SpanReferenceType::FollowsFromRef, nullptr },
    { opentracing::SpanReferenceType::FollowsFromRef, span_context1 },
    { opentracing::SpanReferenceType::ChildOfRef, span_context2 }
  };

  links = shim::utils::makeReferenceLinks(options);
  ASSERT_EQ(links.size(), 2);
  ASSERT_EQ(links[0].first, span_context_shim1->context());
  ASSERT_FALSE(links[0].second.empty());
  ASSERT_EQ(links[0].second[0].first, "opentracing.ref_type");
  ASSERT_EQ(nostd::get<nostd::string_view>(links[0].second[0].second), "follows_from");
  ASSERT_EQ(links[1].first, span_context_shim2->context());
  ASSERT_EQ(links[1].second[0].first, "opentracing.ref_type");
  ASSERT_EQ(nostd::get<nostd::string_view>(links[1].second[0].second), "child_of");
}

TEST_F(ShimUtilsTest, MakeBaggage_EmptyRefs)
{
  auto baggage = baggage::Baggage::GetDefault()->Set("foo", "bar");
  std::string value;
  ASSERT_TRUE(baggage->GetValue("foo", value));
  ASSERT_EQ(value, "bar");

  auto context = context::RuntimeContext::GetCurrent();
  auto new_context = baggage::SetBaggage(context, baggage);
  auto token = context::RuntimeContext::Attach(new_context);
  ASSERT_EQ(context::RuntimeContext::GetCurrent(), new_context);

  opentracing::StartSpanOptions options;
  auto new_baggage = shim::utils::makeBaggage(options);
  ASSERT_TRUE(new_baggage->GetValue("foo", value));
  ASSERT_EQ(value, "bar");
}

TEST_F(ShimUtilsTest, MakeBaggage_NonEmptyRefs)
{
  auto span_context_shim1 = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), 
    baggage::Baggage::GetDefault()->Set("test", "foo")->Set("test1", "hello")));
  auto span_context1 = static_cast<opentracing::SpanContext*>(span_context_shim1.get());
  auto span_context_shim2 = nostd::shared_ptr<shim::SpanContextShim>(new shim::SpanContextShim(
    trace_api::SpanContext::GetInvalid(), 
    baggage::Baggage::GetDefault()->Set("test", "bar")->Set("test2", "world")));
  auto span_context2 = static_cast<opentracing::SpanContext*>(span_context_shim2.get());

  opentracing::StartSpanOptions options;
  options.references = {
    { opentracing::SpanReferenceType::FollowsFromRef, span_context1 },
    { opentracing::SpanReferenceType::ChildOfRef, span_context2 }
  };

  auto baggage = shim::utils::makeBaggage(options);
  std::string value;
  ASSERT_TRUE(baggage->GetValue("test", value));
  ASSERT_EQ(value, "foo");
  ASSERT_TRUE(baggage->GetValue("test1", value));
  ASSERT_EQ(value, "hello");
  ASSERT_TRUE(baggage->GetValue("test2", value));
  ASSERT_EQ(value, "world");
}

TEST_F(ShimUtilsTest, MakeTags)
{
  opentracing::StartSpanOptions options;
  auto attributes = shim::utils::makeTags(options);
  ASSERT_TRUE(attributes.empty());

  options.tags = {{ "foo", 42.0 }, { "bar", true }, { "baz", "test" }};
  attributes = shim::utils::makeTags(options);
  ASSERT_EQ(attributes.size(), 3);
  ASSERT_EQ(attributes[0].first, "foo");
  ASSERT_EQ(nostd::get<double>(attributes[0].second), 42.0);
  ASSERT_EQ(attributes[1].first, "bar");
  ASSERT_TRUE(nostd::get<bool>(attributes[1].second));
  ASSERT_EQ(attributes[2].first, "baz");
  ASSERT_STREQ(nostd::get<const char *>(attributes[2].second), "test" );
}