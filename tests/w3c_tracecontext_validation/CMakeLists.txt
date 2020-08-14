add_library(
  tracecontext_validation
  tracecontext_client.h
  tracecontext_server.h
  tracecontext_validate.cc
  ../../ext/include/opentelemetry/ext/http/server/file_http_server.h
  ../../ext/include/opentelemetry/ext/http/server/http_server.h
  ../../ext/include/opentelemetry/ext/http/server/socket_tools.h)

target_include_directories(tracecontext_validation PUBLIC ../../ext/include)

target_link_libraries(tracecontext_validation opentelemetry_api
                      opentelemetry_trace)