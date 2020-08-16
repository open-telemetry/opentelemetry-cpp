add_library(
  opentelemetry_validation
  tracez_processor.cc
  tracez_data_aggregator.cc
  ../../include/opentelemetry/ext/tracecontext_validation/tracecontext_client.h
  ../../include/opentelemetry/ext/tracecontext_validation/tracecontext_server.h)

target_include_directories(opentelemetry_validation PUBLIC ../../include)

target_link_libraries(opentelemetry_validation opentelemetry_api
                      opentelemetry_trace)
