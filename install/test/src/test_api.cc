// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <string>
#include <unordered_map>

#include <opentelemetry/version.h>

#include <opentelemetry/logs/provider.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/trace/provider.h>

#include <opentelemetry/trace/scope.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/tracer.h>

#include <opentelemetry/logs/log_record.h>
#include <opentelemetry/logs/logger.h>

#include <opentelemetry/metrics/async_instruments.h>
#include <opentelemetry/metrics/meter.h>
#include <opentelemetry/metrics/sync_instruments.h>

#include <opentelemetry/metrics/multi_observer_result.h>

#include <opentelemetry/baggage/baggage.h>
#include <opentelemetry/baggage/baggage_context.h>

#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>

#include <opentelemetry/semconv/client_attributes.h>
#include <opentelemetry/semconv/code_attributes.h>
#include <opentelemetry/semconv/container_attributes.h>
#include <opentelemetry/semconv/db_attributes.h>
#include <opentelemetry/semconv/db_metrics.h>
#include <opentelemetry/semconv/deployment_attributes.h>
#include <opentelemetry/semconv/error_attributes.h>
#include <opentelemetry/semconv/exception_attributes.h>
#include <opentelemetry/semconv/exception_events.h>
#include <opentelemetry/semconv/http_attributes.h>
#include <opentelemetry/semconv/http_metrics.h>
#include <opentelemetry/semconv/k8s_attributes.h>
#include <opentelemetry/semconv/network_attributes.h>
#include <opentelemetry/semconv/otel_attributes.h>
#include <opentelemetry/semconv/schema_url.h>
#include <opentelemetry/semconv/server_attributes.h>
#include <opentelemetry/semconv/service_attributes.h>
#include <opentelemetry/semconv/telemetry_attributes.h>
#include <opentelemetry/semconv/url_attributes.h>
#include <opentelemetry/semconv/user_agent_attributes.h>

TEST(ApiInstallTest, VersionCheck)
{
  EXPECT_GE(OPENTELEMETRY_VERSION_MAJOR, 0);
  EXPECT_GE(OPENTELEMETRY_VERSION_MINOR, 0);
  EXPECT_GE(OPENTELEMETRY_VERSION_PATCH, 0);
  EXPECT_NE(OPENTELEMETRY_VERSION, "not a version");
}

TEST(ApiInstallTest, TraceApiCheck)
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  ASSERT_TRUE(provider != nullptr);

  auto tracer = provider->GetTracer("test-tracer");
  ASSERT_TRUE(tracer != nullptr);

  auto span = tracer->StartSpan("test-span");
  ASSERT_TRUE(span != nullptr);

  auto scope = opentelemetry::trace::Tracer::WithActiveSpan(span);
  span->AddEvent("test-event");
  span->SetAttribute("test-attribute", "test-value");
  span->SetStatus(opentelemetry::trace::StatusCode::kOk, "test-status");
  span->End();
}

TEST(ApiInstallTest, LogsApiCheck)
{
  auto provider = opentelemetry::logs::Provider::GetLoggerProvider();
  ASSERT_TRUE(provider != nullptr);

  auto logger = provider->GetLogger("test-logger");
  ASSERT_TRUE(logger != nullptr);

  auto record = logger->CreateLogRecord();
  ASSERT_TRUE(record != nullptr);
  record->SetSeverity(opentelemetry::logs::Severity::kInfo);
  record->SetBody("test-body");
  record->SetAttribute("test-attribute", "test-value");
  logger->EmitLogRecord(std::move(record));
}

TEST(ApiInstallTest, MetricsApiCheck)
{
  auto provider = opentelemetry::metrics::Provider::GetMeterProvider();
  ASSERT_TRUE(provider != nullptr);

  auto meter = provider->GetMeter("test-meter");
  ASSERT_TRUE(meter != nullptr);

  auto counter = meter->CreateUInt64Counter("test-counter");
  ASSERT_TRUE(counter != nullptr);
  counter->Add(1, {{"test-attribute", "test-value"}});

  auto histogram = meter->CreateDoubleHistogram("test-histogram");
  ASSERT_TRUE(histogram != nullptr);
  histogram->Record(1, {{"test-attribute", "test-value"}},
                    opentelemetry::context::RuntimeContext::GetCurrent());

  auto async_gauge = meter->CreateDoubleObservableGauge("test-gauge");
  ASSERT_TRUE(async_gauge != nullptr);

  auto async_counter = meter->CreateDoubleObservableCounter("test-async-counter");
  ASSERT_TRUE(async_counter != nullptr);

  auto async_updown_counter =
      meter->CreateInt64ObservableUpDownCounter("test-async-updown-counter");
  ASSERT_TRUE(async_updown_counter != nullptr);
}

TEST(ApiInstallTest, BaggageApiCheck)
{
  auto baggage = opentelemetry::baggage::Baggage::GetDefault();
  ASSERT_TRUE(baggage != nullptr);

  auto baggage_with_entry = baggage->Set("key", "value");
  ASSERT_TRUE(baggage_with_entry != nullptr);

  std::string retrieved_value;
  EXPECT_TRUE(baggage_with_entry->GetValue("key", retrieved_value));
  EXPECT_EQ(retrieved_value, "value");

  auto context           = opentelemetry::context::RuntimeContext::GetCurrent();
  auto ctx_with_baggage  = opentelemetry::baggage::SetBaggage(context, baggage_with_entry);
  auto retrieved_baggage = opentelemetry::baggage::GetBaggage(ctx_with_baggage);
  ASSERT_TRUE(retrieved_baggage != nullptr);

  retrieved_value.clear();
  EXPECT_TRUE(retrieved_baggage->GetValue("key", retrieved_value));
  EXPECT_EQ(retrieved_value, "value");
}

TEST(ApiInstallTest, ContextPropagationApiCheck)
{
  struct MapCarrier : public opentelemetry::context::propagation::TextMapCarrier
  {
    std::unordered_map<std::string, std::string> headers;

    opentelemetry::nostd::string_view Get(
        opentelemetry::nostd::string_view key) const noexcept override
    {
      auto it = headers.find(std::string(key));
      if (it != headers.end())
      {
        return it->second;
      }
      return "";
    }
    void Set(opentelemetry::nostd::string_view key,
             opentelemetry::nostd::string_view value) noexcept override
    {
      headers[std::string(key)] = std::string(value);
    }
  };

  auto propagator =
      opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  ASSERT_TRUE(propagator != nullptr);

  MapCarrier carrier;
  auto context =
      opentelemetry::context::RuntimeContext::GetCurrent().SetValue("test-key", "test-value");
  propagator->Inject(carrier, context);
  auto extracted = propagator->Extract(carrier, context);
  EXPECT_EQ(extracted, context);
  EXPECT_TRUE(extracted.HasKey("test-key"));
}

TEST(SemconvInstallTest, SchemaUrl)
{
  EXPECT_NE(opentelemetry::semconv::kSchemaUrl, nullptr);
  EXPECT_STRNE(opentelemetry::semconv::kSchemaUrl, "");
}

TEST(SemconvInstallTest, HttpMetrics)
{
  auto provider = opentelemetry::metrics::Provider::GetMeterProvider();
  ASSERT_TRUE(provider != nullptr);

  auto meter = provider->GetMeter("test-semconv-meter");
  ASSERT_TRUE(meter != nullptr);

  auto http_client_duration =
      opentelemetry::semconv::http::CreateSyncDoubleMetricHttpClientRequestDuration(meter.get());
  ASSERT_TRUE(http_client_duration != nullptr);

  auto http_server_duration =
      opentelemetry::semconv::http::CreateSyncInt64MetricHttpServerRequestDuration(meter.get());
  ASSERT_TRUE(http_server_duration != nullptr);
}

TEST(SemconvInstallTest, DbMetrics)
{
  auto provider = opentelemetry::metrics::Provider::GetMeterProvider();
  ASSERT_TRUE(provider != nullptr);

  auto meter = provider->GetMeter("test-semconv-meter");
  ASSERT_TRUE(meter != nullptr);

  auto db_client_duration =
      opentelemetry::semconv::db::CreateSyncDoubleMetricDbClientOperationDuration(meter.get());
  ASSERT_TRUE(db_client_duration != nullptr);
}
