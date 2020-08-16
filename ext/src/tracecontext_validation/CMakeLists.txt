add_library(
  opentelemetry_validation
  tracecontext_client.cc
  tracecontext_server.cc
  ../../include/opentelemetry/ext/tracecontext_validation/tracecontext_client.h
  ../../include/opentelemetry/ext/tracecontext_validation/tracecontext_server.h)

target_include_directories(opentelemetry_validation PUBLIC ../../include)

target_link_libraries(opentelemetry_validation opentelemetry_api
                      opentelemetry_trace)
