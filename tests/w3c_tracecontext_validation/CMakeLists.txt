add_library(
  opentelemetry_w3c_validation
  tracecontext_client.h
  tracecontext_server.h
  tracecontext_validate.cc
  ../../ext/include/opentelemetry/ext/http/server/file_http_server.h
  ../../ext/include/opentelemetry/ext/http/server/http_server.h
  ../../ext/include/opentelemetry/ext/http/server/socket_tools.h)

target_include_directories(opentelemetry_w3c_validation PUBLIC ../../ext/include)

target_link_libraries(opentelemetry_w3c_validation opentelemetry_api
                      opentelemetry_trace)