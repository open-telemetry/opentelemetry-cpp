add_library(
  opentelemetry_logs
  logger_provider.cc
  logger_provider_factory.cc
  logger.cc
  simple_log_processor.cc
  simple_log_processor_factory.cc
  batch_log_processor.cc
  batch_log_processor_factory.cc
  logger_context.cc
  multi_log_processor.cc
  multi_log_processor_factory.cc
  multi_recordable.cc)

set_target_properties(opentelemetry_logs PROPERTIES EXPORT_NAME logs)

target_link_libraries(opentelemetry_logs PUBLIC opentelemetry_resources
                                                opentelemetry_common)

target_include_directories(
  opentelemetry_logs
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_logs
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
