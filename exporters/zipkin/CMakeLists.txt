# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_directories(include)
add_definitions(-DWITH_CURL)
add_library(
  opentelemetry_exporter_zipkin_trace
  src/zipkin_exporter.cc src/zipkin_exporter_factory.cc src/recordable.cc)

target_include_directories(
  opentelemetry_exporter_zipkin_trace
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

set_target_version(opentelemetry_exporter_zipkin_trace)

target_link_libraries(
  opentelemetry_exporter_zipkin_trace
  PUBLIC opentelemetry_trace opentelemetry_http_client_curl
         nlohmann_json::nlohmann_json)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_exporter_zipkin_trace
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  install(
    DIRECTORY include/opentelemetry/exporters/zipkin
    DESTINATION include/opentelemetry/exporters
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "recordable.h" EXCLUDE)
endif()

if(BUILD_TESTING)
  add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)

  add_executable(zipkin_recordable_test test/zipkin_recordable_test.cc)

  target_link_libraries(
    zipkin_recordable_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_zipkin_trace opentelemetry_resources)

  gtest_add_tests(
    TARGET zipkin_recordable_test
    TEST_PREFIX exporter.
    TEST_LIST zipkin_recordable_test)

  if(MSVC)
    if(GMOCK_LIB)
      unset(GMOCK_LIB CACHE)
    endif()
  endif()
  if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(GMOCK_LIB gmockd PATH_SUFFIXES lib)
  else()
    find_library(GMOCK_LIB gmock PATH_SUFFIXES lib)
  endif()

  add_executable(zipkin_exporter_test test/zipkin_exporter_test.cc)

  target_link_libraries(
    zipkin_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    ${GMOCK_LIB} opentelemetry_exporter_zipkin_trace opentelemetry_resources)

  gtest_add_tests(
    TARGET zipkin_exporter_test
    TEST_PREFIX exporter.
    TEST_LIST zipkin_exporter_test)
endif() # BUILD_TESTING
