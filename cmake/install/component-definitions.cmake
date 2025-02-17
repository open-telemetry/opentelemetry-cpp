# ----------------------------------------------------------------------
# opentelmetry-cpp COMPONENT list
# ----------------------------------------------------------------------
set(opentelemetry-cpp_COMPONENTS
    api
    sdk
    ext_common
    ext_http_curl
    exporters_in_memory
    exporters_ostream
    exporters_otlp_common
    exporters_otlp_file
    exporters_otlp_grpc
    exporters_otlp_http
    exporters_prometheus
    exporters_elasticsearch
    exporters_etw
    exporters_zipkin
    shims_opentracing
    ext_dll
)

# ----------------------------------------------------------------------
# COMPONENT to TARGET lists
# ----------------------------------------------------------------------

# COMPONENT api
set(COMPONENT_opentelemetry-cpp_api_TARGETS
    opentelemetry-cpp::api
)

# COMPONENT sdk
set(COMPONENT_opentelemetry-cpp_sdk_TARGETS
    opentelemetry-cpp::sdk
    opentelemetry-cpp::version
    opentelemetry-cpp::common
    opentelemetry-cpp::resources
    opentelemetry-cpp::trace
    opentelemetry-cpp::metrics
    opentelemetry-cpp::logs
)

# COMPONENT ext_common
set(COMPONENT_opentelemetry-cpp_ext_common_TARGETS
    opentelemetry-cpp::ext
)

# COMPONENT ext_http_curl
set(COMPONENT_opentelemetry-cpp_ext_http_curl_TARGETS
    opentelemetry-cpp::http_client_curl
)

# COMPONENT ext_dll
set(COMPONENT_opentelemetry-cpp_ext_dll_TARGETS
    opentelemetry-cpp::opentelemetry_cpp
)

# COMPONENT exporters_in_memory
set(COMPONENT_opentelemetry-cpp_exporters_in_memory_TARGETS
    opentelemetry-cpp::in_memory_span_exporter
    opentelemetry-cpp::in_memory_metric_exporter
)

# COMPONENT exporters_ostream
set(COMPONENT_opentelemetry-cpp_exporters_ostream_TARGETS
    opentelemetry-cpp::ostream_log_record_exporter
    opentelemetry-cpp::ostream_metrics_exporter
    opentelemetry-cpp::ostream_span_exporter
)

# COMPONENT exporters_otlp_common
set(COMPONENT_opentelemetry-cpp_exporters_otlp_common_TARGETS
    opentelemetry-cpp::proto
    opentelemetry-cpp::otlp_recordable
)

# COMPONENT exporters_otlp_file
set(COMPONENT_opentelemetry-cpp_exporters_otlp_file_TARGETS
    opentelemetry-cpp::otlp_file_client
    opentelemetry-cpp::otlp_file_exporter
    opentelemetry-cpp::otlp_file_log_record_exporter
    opentelemetry-cpp::otlp_file_metric_exporter
)

# COMPONENT exporters_otlp_grpc
set(COMPONENT_opentelemetry-cpp_exporters_otlp_grpc_TARGETS
    opentelemetry-cpp::proto_grpc
    opentelemetry-cpp::otlp_grpc_client
    opentelemetry-cpp::otlp_grpc_exporter
    opentelemetry-cpp::otlp_grpc_log_record_exporter
    opentelemetry-cpp::otlp_grpc_metrics_exporter
)

# COMPONENT exporters_otlp_http
set(COMPONENT_opentelemetry-cpp_exporters_otlp_http_TARGETS
    opentelemetry-cpp::otlp_http_client
    opentelemetry-cpp::otlp_http_exporter
    opentelemetry-cpp::otlp_http_log_record_exporter
    opentelemetry-cpp::otlp_http_metric_exporter
)

# COMPONENT exporters_prometheus
set(COMPONENT_opentelemetry-cpp_exporters_prometheus_TARGETS
    opentelemetry-cpp::prometheus_exporter
)

# COMPONENT exporters_elasticsearch
set(COMPONENT_opentelemetry-cpp_exporters_elasticsearch_TARGETS
    opentelemetry-cpp::elasticsearch_log_record_exporter
)

# COMPONENT exporters_etw
set(COMPONENT_opentelemetry-cpp_exporters_etw_TARGETS
    opentelemetry-cpp::etw_exporter
)

# COMPONENT exporters_zipkin
set(COMPONENT_opentelemetry-cpp_exporters_zipkin_TARGETS
    opentelemetry-cpp::zipkin_trace_exporter
)

# COMPONENT shims_opentracing
set(COMPONENT_opentelemetry-cpp_shims_opentracing_TARGETS
    opentelemetry-cpp::opentracing_shim
)


#-----------------------------------------------------------------------
# COMPONENT to COMPONENT dependencies
#-----------------------------------------------------------------------

# COMPONENT sdk depends on COMPONENT api
set(COMPONENT_sdk_COMPONENT_DEPENDS
    api
)

set(COMPONENT_ext_common_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_ext_http_curl_COMPONENT_DEPENDS
    api
    sdk
    ext_common
)

set(COMPONENT_ext_dll_COMPONENT_DEPENDS
    api
    sdk
    ext_common
    ext_http_curl
    exporters_in_memory
    exporters_ostream
    exporters_otlp_common
    exporters_otlp_file
    exporters_otlp_grpc
    exporters_otlp_http
    exporters_prometheus
    exporters_elasticsearch
    exporters_etw
    exporters_zipkin
    shims_opentracing
)

set(COMPONENT_exporters_in_memory_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_exporters_ostream_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_exporters_otlp_common_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_exporters_otlp_file_COMPONENT_DEPENDS
    api
    sdk
    exporters_otlp_common
)

set(COMPONENT_exporters_otlp_grpc_COMPONENT_DEPENDS
    api
    sdk
    ext_common
    exporters_otlp_common
)

set(COMPONENT_exporters_otlp_http_COMPONENT_DEPENDS
    api
    sdk
    ext_common
    ext_http_curl
    exporters_otlp_common
)

set(COMPONENT_exporters_prometheus_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_exporters_elasticsearch_COMPONENT_DEPENDS
    api
    sdk
    ext_common
    ext_http_curl
)

set(COMPONENT_exporters_etw_COMPONENT_DEPENDS
    api
    sdk
)

set(COMPONENT_exporters_zipkin_COMPONENT_DEPENDS
    api
    sdk
    ext_common
    ext_http_curl
)

set(COMPONENT_shims_opentracing_COMPONENT_DEPENDS
    api
)