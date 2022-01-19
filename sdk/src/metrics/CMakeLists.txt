add_library(
  opentelemetry_metrics
  meter_provider.cc
  meter.cc
  meter_context.cc
  view/sum_aggregation.cc
  view/lastvalue_aggregation.cc
  view/histogram_aggregation.cc
  view/drop_aggregation.cc
  view/default_aggregation.cc)

set_target_properties(opentelemetry_metrics PROPERTIES EXPORT_NAME metrics)

target_link_libraries(opentelemetry_metrics PUBLIC opentelemetry_common)

target_include_directories(
  opentelemetry_metrics
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_metrics
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
