add_library(
  opentelemetry_zpages
  tracez_processor.cc
  tracez_shared_data.cc
  tracez_data_aggregator.cc
  ../../include/opentelemetry/ext/zpages/tracez_shared_data.h
  ../../include/opentelemetry/ext/zpages/tracez_processor.h
  ../../include/opentelemetry/ext/zpages/tracez_data_aggregator.h
  ../../include/opentelemetry/ext/zpages/tracez_http_server.h)

set_target_properties(opentelemetry_zpages PROPERTIES EXPORT_NAME zpages)

target_link_libraries(opentelemetry_zpages PUBLIC opentelemetry_ext
                                                  opentelemetry_trace)

install(
  TARGETS opentelemetry_zpages
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
