# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(opentelemetry_sdk INTERFACE)
target_include_directories(
  opentelemetry_sdk
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_sdk PROPERTIES EXPORT_NAME sdk)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_sdk
    EXPORT "${PROJECT_NAME}-sdk-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT sdk)

  install(
    DIRECTORY include/opentelemetry/
    DESTINATION include/opentelemetry
    COMPONENT sdk
    FILES_MATCHING
    PATTERN "*config.h")

  install(
    DIRECTORY include/opentelemetry/sdk
    DESTINATION include/opentelemetry
    COMPONENT sdk
    FILES_MATCHING
    PATTERN "*.h")

  install(
    EXPORT "${PROJECT_NAME}-sdk-target"
    FILE "${PROJECT_NAME}-sdk-target.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT sdk)

endif()

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
