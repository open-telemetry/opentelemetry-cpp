add_library(
  tracecontext_validation
  tracecontext_validate.cc)

target_include_directories(tracecontext_validation PUBLIC ../../ext/include)

target_link_libraries(tracecontext_validation opentelemetry_api
                      opentelemetry_trace)