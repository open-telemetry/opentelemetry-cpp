// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/resource_detector.h"

#include "custom_resource_detector.h"

opentelemetry::sdk::resource::Resource CustomResourceDetector::Detect() noexcept
{
  OTEL_INTERNAL_LOG_INFO("CustomResourceDetector::Detect(): YOUR CODE HERE");
  return ResourceDetector::Create({{"custom.comment", comment_}});
}
