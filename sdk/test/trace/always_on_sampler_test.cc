#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"

#include <gtest/gtest.h>
#include <map>

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::nostd;

TEST(AlwaysOnSampler, ShouldSample)
{
  AlwaysOnSampler sampler;

  // A buffer of trace_id with no specific meaning
  constexpr uint8_t buf[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

  trace_api::TraceId trace_id_invalid;
  trace_api::TraceId trace_id_valid(buf);
  std::map<std::string, int> key_value_container = {{"key", 0}};

  // Test with invalid (empty) trace id and empty parent context
  auto sampling_result = sampler.ShouldSample(
      nullptr, trace_id_invalid, "invalid trace id test", trace_api::SpanKind::kServer,
      trace_api::KeyValueIterableView<std::map<std::string, int>>(key_value_container));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);

  // Test with a valid trace id and empty parent context
  sampling_result = sampler.ShouldSample(
      nullptr, trace_id_valid, "valid trace id test", trace_api::SpanKind::kServer,
      trace_api::KeyValueIterableView<std::map<std::string, int>>(key_value_container));

  ASSERT_EQ(Decision::RECORD_AND_SAMPLE, sampling_result.decision);
  ASSERT_EQ(nullptr, sampling_result.attributes);
}

TEST(AlwaysOnSampler, GetDescription)
{
  AlwaysOnSampler sampler;

  ASSERT_EQ("AlwaysOnSampler", sampler.GetDescription());
}
