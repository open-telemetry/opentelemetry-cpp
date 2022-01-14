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

template<class T>
class Aggregator
{
  public:
    virtual T CreateAccumulation() noexcept = 0;

    virtual T Merge(T& prev, T& current ) noexcept = 0;

    virtual T diff(T& prev, T& current) noexcept = 0;

};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif