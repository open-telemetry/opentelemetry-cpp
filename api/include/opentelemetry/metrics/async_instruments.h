// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
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

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
