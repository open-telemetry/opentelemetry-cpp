// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>

#  include <cstddef>
#  include <cstdint>
#  include <memory>
#  include <string>
#  include <vector>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/function_ref.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace
{

class CountingKeyValueIterable final : public opentelemetry::common::KeyValueIterable
{
public:
  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback)
      const noexcept override
  {
    ++for_each_calls_;
    return callback("key", nostd::string_view{"value"});
  }

  size_t size() const noexcept override { return 1; }

  size_t GetForEachCalls() const noexcept { return for_each_calls_; }

private:
  mutable size_t for_each_calls_ = 0;
};

class DeterministicSelector final : public ReservoirCellSelector
{
public:
  explicit DeterministicSelector(bool accept) noexcept : accept_{accept} {}

  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            int64_t /* value */,
                            const MetricAttributes & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    return accept_ ? 0 : -1;
  }

  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            double /* value */,
                            const MetricAttributes & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    return accept_ ? 0 : -1;
  }

  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            int64_t /* value */,
                            const opentelemetry::common::KeyValueIterable & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    return accept_ ? 0 : -1;
  }

  int ReservoirCellIndexFor(const std::vector<ReservoirCell> & /* cells */,
                            double /* value */,
                            const opentelemetry::common::KeyValueIterable & /* attributes */,
                            const opentelemetry::context::Context & /* context */) override
  {
    return accept_ ? 0 : -1;
  }

  void reset() override {}

private:
  bool accept_;
};

class OwnedOnlyReservoir final : public ExemplarReservoir
{
public:
  using ExemplarReservoir::OfferMeasurement;

  void OfferMeasurement(int64_t /* value */,
                        const MetricAttributes & /* attributes */,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {}

  void OfferMeasurement(double /* value */,
                        const MetricAttributes & /* attributes */,
                        const opentelemetry::context::Context & /* context */) noexcept override
  {}

  std::vector<std::shared_ptr<ExemplarData>> CollectAndReset(
      const MetricAttributes & /* point_attributes */) noexcept override
  {
    return {};
  }
};

TEST(ExemplarAttributeOffer, RejectedLongOfferDoesNotMaterializeAttributes)
{
  auto selector = std::shared_ptr<ReservoirCellSelector>{new DeterministicSelector{false}};
  FixedSizeExemplarReservoir reservoir{1, selector, &ReservoirCell::GetAndResetLong};
  CountingKeyValueIterable attributes;

  reservoir.OfferMeasurement(static_cast<int64_t>(1), attributes,
                             opentelemetry::context::Context{});

  EXPECT_EQ(attributes.GetForEachCalls(), 0);
}

TEST(ExemplarAttributeOffer, RejectedDoubleOfferDoesNotMaterializeAttributes)
{
  auto selector = std::shared_ptr<ReservoirCellSelector>{new DeterministicSelector{false}};
  FixedSizeExemplarReservoir reservoir{1, selector, &ReservoirCell::GetAndResetDouble};
  CountingKeyValueIterable attributes;

  reservoir.OfferMeasurement(1.0, attributes, opentelemetry::context::Context{});

  EXPECT_EQ(attributes.GetForEachCalls(), 0);
}

TEST(ExemplarAttributeOffer, AcceptedOfferMaterializesAttributesAfterSelection)
{
  auto selector = std::shared_ptr<ReservoirCellSelector>{new DeterministicSelector{true}};
  FixedSizeExemplarReservoir reservoir{1, selector, &ReservoirCell::GetAndResetDouble};
  CountingKeyValueIterable attributes;

  reservoir.OfferMeasurement(1.0, attributes, opentelemetry::context::Context{});

  EXPECT_EQ(attributes.GetForEachCalls(), 1);
}

}  // namespace
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
