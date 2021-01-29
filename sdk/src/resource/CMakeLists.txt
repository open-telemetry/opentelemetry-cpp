add_library(opentelemetry_resources resource.cc resource_detector.cc)

set_target_properties(opentelemetry_resources PROPERTIES EXPORT_NAME resources)

target_link_libraries(opentelemetry_resources opentelemetry_common)

target_include_directories(
  opentelemetry_resources
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_resources
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
