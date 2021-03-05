add_library(opentelemetry_version version.cc)

set_target_properties(opentelemetry_version PROPERTIES EXPORT_NAME version)

target_link_libraries(opentelemetry_version PUBLIC opentelemetry_api
                                                   opentelemetry_sdk)

target_include_directories(
  opentelemetry_version
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_version
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
