# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Configured from opentelemetry-cpp/cmake/component-definitions.cmake.in

# ----------------------------------------------------------------------
# opentelmetry-cpp Built COMPONENT list
# ----------------------------------------------------------------------
set(OTEL_BUILT_COMPONENTS_LIST api;sdk;ext_common;exporters_ostream;exporters_ostream_builder;exporters_in_memory)

# ----------------------------------------------------------------------
# COMPONENT to TARGET lists
# ----------------------------------------------------------------------

# COMPONENT api
set(COMPONENT_api_TARGETS
    opentelemetry-cpp::api
)

# COMPONENT sdk
set(COMPONENT_sdk_TARGETS
    opentelemetry-cpp::sdk
    opentelemetry-cpp::common
    opentelemetry-cpp::resources
    opentelemetry-cpp::version
    opentelemetry-cpp::logs
    opentelemetry-cpp::trace
    opentelemetry-cpp::metrics
)

# COMPONENT ext_common
set(COMPONENT_ext_common_TARGETS
    opentelemetry-cpp::ext
)

# COMPONENT exporters_ostream
set(COMPONENT_exporters_ostream_TARGETS
    opentelemetry-cpp::ostream_span_exporter
    opentelemetry-cpp::ostream_metrics_exporter
    opentelemetry-cpp::ostream_log_record_exporter
)

# COMPONENT exporters_ostream_builder
set(COMPONENT_exporters_ostream_builder_TARGETS
    opentelemetry-cpp::ostream_span_exporter_builder
    opentelemetry-cpp::ostream_metrics_exporter_builder
    opentelemetry-cpp::ostream_log_record_exporter_builder
)

# COMPONENT exporters_in_memory
set(COMPONENT_exporters_in_memory_TARGETS
    opentelemetry-cpp::in_memory_span_exporter
    opentelemetry-cpp::in_memory_metric_exporter
)



#-----------------------------------------------------------------------
# COMPONENT to COMPONENT dependencies
#-----------------------------------------------------------------------

# COMPONENT api internal dependencies
set(COMPONENT_api_COMPONENT_DEPENDS
)

# COMPONENT sdk internal dependencies
set(COMPONENT_sdk_COMPONENT_DEPENDS
    api
)

# COMPONENT ext_common internal dependencies
set(COMPONENT_ext_common_COMPONENT_DEPENDS
    api
)

# COMPONENT exporters_ostream internal dependencies
set(COMPONENT_exporters_ostream_COMPONENT_DEPENDS
    api
    sdk
)

# COMPONENT exporters_ostream_builder internal dependencies
set(COMPONENT_exporters_ostream_builder_COMPONENT_DEPENDS
    api
    sdk
    exporters_ostream
)

# COMPONENT exporters_in_memory internal dependencies
set(COMPONENT_exporters_in_memory_COMPONENT_DEPENDS
    api
    sdk
)




#-----------------------------------------------------------------------
# COMPONENT to THIRDPARTY dependencies
#-----------------------------------------------------------------------

# COMPONENT api thirdparty dependencies
set(COMPONENT_api_THIRDPARTY_DEPENDS
)

# COMPONENT sdk thirdparty dependencies
set(COMPONENT_sdk_THIRDPARTY_DEPENDS
    Threads
)

# COMPONENT ext_common thirdparty dependencies
set(COMPONENT_ext_common_THIRDPARTY_DEPENDS
)

# COMPONENT exporters_ostream thirdparty dependencies
set(COMPONENT_exporters_ostream_THIRDPARTY_DEPENDS
)

# COMPONENT exporters_ostream_builder thirdparty dependencies
set(COMPONENT_exporters_ostream_builder_THIRDPARTY_DEPENDS
)

# COMPONENT exporters_in_memory thirdparty dependencies
set(COMPONENT_exporters_in_memory_THIRDPARTY_DEPENDS
)


