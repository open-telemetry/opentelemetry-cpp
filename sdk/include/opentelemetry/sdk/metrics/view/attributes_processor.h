// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class AttributesProcessor 
{
public:
        virtual void process() = 0;
        
};
}
}
OPENTELEMETRY_END_NAMESPACE