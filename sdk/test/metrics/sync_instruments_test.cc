// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/instruments.h"

TEST(SyncInstruments, LongCounter)
{
  InstrumentDescriptor instr_desc = {};
}

#endif