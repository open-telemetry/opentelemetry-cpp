add_library(opentelemetry_metrics_deprecated meter_provider.cc meter.cc
                                             ungrouped_processor.cc)

set_target_properties(opentelemetry_metrics_deprecated
                      PROPERTIES EXPORT_NAME metrics_deprecated)

target_link_libraries(opentelemetry_metrics_deprecated
                      PUBLIC opentelemetry_common)

target_include_directories(
  opentelemetry_metrics_deprecated
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_metrics_deprecated
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
