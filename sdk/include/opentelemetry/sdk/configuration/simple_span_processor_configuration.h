// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SimpleSpanProcessorConfiguration : public SpanProcessorConfiguration
{
public:
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
