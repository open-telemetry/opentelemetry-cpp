// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif