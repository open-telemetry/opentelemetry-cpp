// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
namespace sdk
{
namespace metrics
{

class MeterSelector {
    public:

    MeterSelector(nostd::string_view name, nostd::string_view version, nostd::string_view schema){
        name_filter_ =new ExactPredicate(name);
        version_filter_ = new ExactPredicate(version);
        schema_filter_ = new ExactPredicate(schema);
    }

    // Returns name filter predicate. This shouldn't be deleted
    const Predicate* const GetNameFilter(){
        return name_filter_.get();
    }
    
    // Returns version filter predicate. This shouldn't be deleted
    const Predicate* const GetVersionFilter() {
        return version_filter_get();
    }

    // Returns schema filter predicate. This shouldn't be deleted
    const Predicate* const GetSchemaFilter() {
        return schema_filter_.get();
    }

    private:
    std::unique_ptr<Predicate> name_filter_;
    std::unique_ptr<Predicate> version_filter_;
    std::unique_ptr<Predicate> schema_filter_;

};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif