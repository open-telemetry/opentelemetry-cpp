add_library(
  opentelemetry_zpages
  tracez_processor.cc tracez_data_aggregator.cc
  ../../include/opentelemetry/ext/zpages/tracez_processor.h
  ../../include/opentelemetry/ext/zpages/tracez_data_aggregator.h)

target_include_directories(opentelemetry_zpages PUBLIC ../../include)

target_link_libraries(opentelemetry_zpages opentelemetry_api
                      opentelemetry_trace)
