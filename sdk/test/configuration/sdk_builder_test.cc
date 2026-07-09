// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/sdk_builder.h"
#include "opentelemetry/sdk/configuration/span_limits_configuration.h"
#include "opentelemetry/sdk/configuration/tracer_provider_configuration.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

using opentelemetry::sdk::configuration::Registry;
using opentelemetry::sdk::configuration::SdkBuilder;
using opentelemetry::sdk::configuration::SpanLimitsConfiguration;
using opentelemetry::sdk::configuration::TracerProviderConfiguration;

TEST(SdkBuilder, SpanLimitsDefaults)
{
  auto model    = std::make_unique<TracerProviderConfiguration>();
  model->limits = nullptr;

  SdkBuilder builder(std::make_shared<Registry>());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = builder.CreateTracerProvider(model, resource);
  ASSERT_NE(provider, nullptr);

  auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_count_limit,
            opentelemetry::sdk::trace::SpanLimits::kDefaultAttributeCountLimit);
  EXPECT_EQ(limits.event_count_limit,
            opentelemetry::sdk::trace::SpanLimits::kDefaultEventCountLimit);
  EXPECT_EQ(limits.link_count_limit, opentelemetry::sdk::trace::SpanLimits::kDefaultLinkCountLimit);
  EXPECT_EQ(limits.event_attribute_count_limit,
            opentelemetry::sdk::trace::SpanLimits::kDefaultEventAttributeCountLimit);
  EXPECT_EQ(limits.link_attribute_count_limit,
            opentelemetry::sdk::trace::SpanLimits::kDefaultLinkAttributeCountLimit);
  EXPECT_EQ(limits.attribute_value_length_limit,
            opentelemetry::sdk::trace::SpanLimits::kDefaultAttributeValueLengthLimit);
}

TEST(SdkBuilder, SpanLimitsConfiguration)
{
  auto model                                  = std::make_unique<TracerProviderConfiguration>();
  model->limits                               = std::make_unique<SpanLimitsConfiguration>();
  model->limits->attribute_value_length_limit = 1111;
  model->limits->attribute_count_limit        = 2222;
  model->limits->event_count_limit            = 3333;
  model->limits->link_count_limit             = 4444;
  model->limits->event_attribute_count_limit  = 5555;
  model->limits->link_attribute_count_limit   = 6666;

  SdkBuilder builder(std::make_shared<Registry>());
  auto resource = opentelemetry::sdk::resource::Resource::Create({});
  auto provider = builder.CreateTracerProvider(model, resource);
  ASSERT_NE(provider, nullptr);

  auto limits = provider->GetSpanLimits();
  EXPECT_EQ(limits.attribute_value_length_limit, model->limits->attribute_value_length_limit);
  EXPECT_EQ(limits.attribute_count_limit, model->limits->attribute_count_limit);
  EXPECT_EQ(limits.event_count_limit, model->limits->event_count_limit);
  EXPECT_EQ(limits.link_count_limit, model->limits->link_count_limit);
  EXPECT_EQ(limits.event_attribute_count_limit, model->limits->event_attribute_count_limit);
  EXPECT_EQ(limits.link_attribute_count_limit, model->limits->link_attribute_count_limit);
}
