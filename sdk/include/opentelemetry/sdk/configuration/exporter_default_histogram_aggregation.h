// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

enum enum_default_histogram_aggregation
{
  explicit_bucket_histogram,
  base2_exponential_bucket_histogram
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
