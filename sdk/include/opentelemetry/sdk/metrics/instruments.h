// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
enum class InstrumentType {
    kCounter,
    kHistogram,
    kUpDownCounter,
    kObservableCounter,
    kObservableGauge,
    kObservableUpDownCounter
};

enum class InstrumentValueType {
    kInt,
    kLong,
    kFloat,
    kDouble
};

struct InstrumentDescriptor {
    std::string name_;
    std::string description_;
    std::string unit_;
    InstrumentType type_;
    InstrumentValueType valueType_;
};

class InstrumentSelector {
public:
InstrumentSelector(nostd::string name, InstrumentType type): name(name_), type_(type) {}
InstrumentType GetType(){return type_;}
nostd::string GetNameFilter() { return name_;}

private:
std::string name_;
InstrumentType type_;
};

}
OPENTELEMETRY_END_NAMESPACE
#endif
