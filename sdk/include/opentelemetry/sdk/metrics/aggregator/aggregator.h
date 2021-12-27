// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class Aggregator
{
  // TBD
};

class NoOpAggregator : public Aggregator
{
  // TBD
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif