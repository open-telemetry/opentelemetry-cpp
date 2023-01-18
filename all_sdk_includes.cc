#include <opentelemetry/sdk/common/atomic_shared_ptr.h>
#include <opentelemetry/sdk/common/atomic_unique_ptr.h>
#include <opentelemetry/sdk/common/attributemap_hash.h>
#include <opentelemetry/sdk/common/attribute_utils.h>
#include <opentelemetry/sdk/common/circular_buffer.h>
#include <opentelemetry/sdk/common/circular_buffer_range.h>
#include <opentelemetry/sdk/common/empty_attributes.h>
#include <opentelemetry/sdk/common/env_variables.h>
#include <opentelemetry/sdk/common/exporter_utils.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h>
#include <opentelemetry/sdk/instrumentationscope/instrumentation_scope.h>
#include <opentelemetry/sdk/logs/exporter.h>
#include <opentelemetry/sdk/logs/logger.h>
#include <opentelemetry/sdk/logs/logger_context.h>
#include <opentelemetry/sdk/logs/logger_context_factory.h>
#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/multi_recordable.h>
#include <opentelemetry/sdk/logs/processor.h>
#include <opentelemetry/sdk/logs/recordable.h>
#include <opentelemetry/sdk/metrics/async_instruments.h>
#include <opentelemetry/sdk/metrics/instruments.h>
#include <opentelemetry/sdk/metrics/instrument_metadata_validator.h>
#include <opentelemetry/sdk/metrics/meter.h>
#include <opentelemetry/sdk/metrics/meter_context.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/sdk/metrics/metric_reader.h>
#include <opentelemetry/sdk/metrics/observer_result.h>
#include <opentelemetry/sdk/metrics/push_metric_exporter.h>
#include <opentelemetry/sdk/metrics/sync_instruments.h>
#include <opentelemetry/sdk/metrics/aggregation/aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/aggregation_config.h>
#include <opentelemetry/sdk/metrics/aggregation/default_aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/drop_aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/lastvalue_aggregation.h>
#include <opentelemetry/sdk/metrics/aggregation/sum_aggregation.h>
#include <opentelemetry/sdk/metrics/data/exemplar_data.h>
#include <opentelemetry/sdk/metrics/data/metric_data.h>
#include <opentelemetry/sdk/metrics/data/point_data.h>
#include <opentelemetry/sdk/metrics/exemplar/always_sample_filter.h>
#include <opentelemetry/sdk/metrics/exemplar/filter.h>
#include <opentelemetry/sdk/metrics/exemplar/filtered_exemplar_reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/histogram_exemplar_reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/never_sample_filter.h>
#include <opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/reservoir_cell.h>
#include <opentelemetry/sdk/metrics/exemplar/reservoir_cell_selector.h>
#include <opentelemetry/sdk/metrics/exemplar/with_trace_sample_filter.h>
#include <opentelemetry/sdk/metrics/export/metric_producer.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h>
#include <opentelemetry/sdk/metrics/state/async_metric_storage.h>
#include <opentelemetry/sdk/metrics/state/attributes_hashmap.h>
#include <opentelemetry/sdk/metrics/state/metric_collector.h>
#include <opentelemetry/sdk/metrics/state/metric_storage.h>
#include <opentelemetry/sdk/metrics/state/multi_metric_storage.h>
#include <opentelemetry/sdk/metrics/state/observable_registry.h>
#include <opentelemetry/sdk/metrics/state/sync_metric_storage.h>
#include <opentelemetry/sdk/metrics/state/temporal_metric_storage.h>
#include <opentelemetry/sdk/metrics/view/attributes_processor.h>
#include <opentelemetry/sdk/metrics/view/instrument_selector.h>
#include <opentelemetry/sdk/metrics/view/meter_selector.h>
#include <opentelemetry/sdk/metrics/view/predicate.h>
#include <opentelemetry/sdk/metrics/view/predicate_factory.h>
#include <opentelemetry/sdk/metrics/view/view.h>
#include <opentelemetry/sdk/metrics/view/view_registry.h>
#include <opentelemetry/sdk/resource/resource.h>
#include <opentelemetry/sdk/resource/resource_detector.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/trace/batch_span_processor.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_options.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/id_generator.h>
#include <opentelemetry/sdk/trace/multi_recordable.h>
#include <opentelemetry/sdk/trace/multi_span_processor.h>
#include <opentelemetry/sdk/trace/processor.h>
#include <opentelemetry/sdk/trace/random_id_generator.h>
#include <opentelemetry/sdk/trace/random_id_generator_factory.h>
#include <opentelemetry/sdk/trace/recordable.h>
#include <opentelemetry/sdk/trace/sampler.h>
#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/span_data.h>
#include <opentelemetry/sdk/trace/tracer.h>
#include <opentelemetry/sdk/trace/tracer_context.h>
#include <opentelemetry/sdk/trace/tracer_context_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/sdk/trace/samplers/always_off.h>
#include <opentelemetry/sdk/trace/samplers/always_off_factory.h>
#include <opentelemetry/sdk/trace/samplers/always_on.h>
#include <opentelemetry/sdk/trace/samplers/always_on_factory.h>
#include <opentelemetry/sdk/trace/samplers/parent.h>
#include <opentelemetry/sdk/trace/samplers/parent_factory.h>
#include <opentelemetry/sdk/trace/samplers/trace_id_ratio.h>
#include <opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h>
#include <opentelemetry/sdk/version/version.h>
#include <opentelemetry/exporters/memory/in_memory_data.h>
#include <opentelemetry/exporters/memory/in_memory_span_data.h>
#include <opentelemetry/exporters/memory/in_memory_span_exporter.h>
#include <opentelemetry/exporters/memory/in_memory_span_exporter_factory.h>
#include <opentelemetry/exporters/ostream/common_utils.h>
#include <opentelemetry/exporters/ostream/metric_exporter.h>
#include <opentelemetry/exporters/ostream/span_exporter.h>
#include <opentelemetry/exporters/ostream/span_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_environment.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_client.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_utils.h>
#include <opentelemetry/exporters/otlp/otlp_http.h>
#include <opentelemetry/exporters/otlp/otlp_http_client.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h>
#include <opentelemetry/exporters/otlp/otlp_metric_utils.h>
#include <opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h>
//#include <opentelemetry/exporters/otlp/otlp_recordable.h>
#include <opentelemetry/exporters/otlp/otlp_recordable_utils.h>
#include <opentelemetry/exporters/otlp/protobuf_include_prefix.h>
#include <opentelemetry/exporters/otlp/protobuf_include_suffix.h>
#include <opentelemetry/exporters/zipkin/recordable.h>
#include <opentelemetry/exporters/zipkin/zipkin_exporter.h>
#include <opentelemetry/exporters/zipkin/zipkin_exporter_factory.h>
#include <opentelemetry/exporters/zipkin/zipkin_exporter_options.h>
#include <opentelemetry/ext/http/client/http_client.h>
#include <opentelemetry/ext/http/client/http_client_factory.h>
#include <opentelemetry/ext/http/client/curl/http_client_curl.h>
#include <opentelemetry/ext/http/client/curl/http_operation_curl.h>
#include <opentelemetry/ext/http/common/url_parser.h>
//#include <opentelemetry/ext/http/server/file_http_server.h>
#include <opentelemetry/ext/http/server/http_server.h>
#include <opentelemetry/ext/http/server/socket_tools.h>
#include <opentelemetry/ext/zpages/latency_boundaries.h>
#include <opentelemetry/ext/zpages/threadsafe_span_data.h>
#include <opentelemetry/ext/zpages/tracez_data.h>
#include <opentelemetry/ext/zpages/tracez_data_aggregator.h>
#include <opentelemetry/ext/zpages/tracez_http_server.h>
#include <opentelemetry/ext/zpages/tracez_processor.h>
#include <opentelemetry/ext/zpages/tracez_shared_data.h>
#include <opentelemetry/ext/zpages/zpages.h>
#include <opentelemetry/ext/zpages/zpages_http_server.h>
#include <opentelemetry/ext/zpages/static/tracez_index.h>
#include <opentelemetry/ext/zpages/static/tracez_script.h>
#include <opentelemetry/ext/zpages/static/tracez_style.h>

// #include <opentelemetry/exporters/etw/etw_config.h>
// #include <opentelemetry/exporters/etw/etw_fields.h>
// #include <opentelemetry/exporters/etw/etw_logger.h>
// #include <opentelemetry/exporters/etw/etw_logger_exporter.h>
// #include <opentelemetry/exporters/etw/etw_properties.h>
// #include <opentelemetry/exporters/etw/etw_provider.h>
// #include <opentelemetry/exporters/etw/etw_random_id_generator.h>
// #include <opentelemetry/exporters/etw/etw_tail_sampler.h>
// #include <opentelemetry/exporters/etw/etw_traceloggingdynamic.h>
// #include <opentelemetry/exporters/etw/etw_tracer.h>
// #include <opentelemetry/exporters/etw/etw_tracer_exporter.h>
// #include <opentelemetry/exporters/etw/TraceLoggingDynamic.h>
// #include <opentelemetry/exporters/etw/utils.h>
// #include <opentelemetry/exporters/etw/uuid.h>

#include <opentelemetry/exporters/elasticsearch/es_log_recordable.h>
#include <opentelemetry/exporters/elasticsearch/es_log_record_exporter.h>

#include <opentelemetry/exporters/etw/etw_provider.h>
#include <opentelemetry/exporters/etw/etw_logger.h>
#include <opentelemetry/exporters/etw/etw_random_id_generator.h>
#include <opentelemetry/exporters/etw/etw_tracer.h>

