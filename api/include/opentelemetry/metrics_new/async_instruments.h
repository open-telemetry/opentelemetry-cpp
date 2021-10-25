// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "instrument.h"
#  include "observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics_new
{
class AsynchronousInstrument
{};

template <class T>
class ObservableCounter : public AsynchronousInstrument
{};

template <class T>
class ObservableGauge : public AsynchronousInstrument
{};

template <class T>
class ObservableUpDownCounter : public AsynchronousInstrument
{};

}  // namespace metrics_new
OPENTELEMETRY_END_NAMESPACE
#endif
