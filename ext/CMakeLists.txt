# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_ext INTERFACE)
target_include_directories(
  opentelemetry_ext
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_ext PROPERTIES EXPORT_NAME "ext")
target_link_libraries(opentelemetry_ext INTERFACE opentelemetry_api)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_ext
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install(
    DIRECTORY include/opentelemetry/ext
    DESTINATION include/opentelemetry/
    FILES_MATCHING
    PATTERN "*.h")
endif()

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
