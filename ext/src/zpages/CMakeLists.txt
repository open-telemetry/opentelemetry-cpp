add_library(
  opentelemetry_zpages
  tracez_processor.cc ../../include/opentelemetry/ext/zpages/tracez_processor.h)

target_include_directories(opentelemetry_zpages PUBLIC ../../include)

target_link_libraries(opentelemetry_zpages opentelemetry_api
                      opentelemetry_trace)
