// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/measurement_processor.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;
using namespace opentelemetry::sdk::metrics;

std::string library_name     = "opentelemetry-cpp";
std::string library_version  = "0.1.0";
auto instrumentation_library = InstrumentationLibrary::Create(library_name, library_version);
SyncMeasurementProcessor measurement_processor;

using M = std::map<std::string, std::string>;

TEST(AsyncInstruments, LongCounter) {}
