# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_otlp_recordable
  src/otlp_log_recordable.cc src/otlp_recordable.cc
  src/otlp_populate_attribute_utils.cc src/otlp_recordable_utils.cc
  src/otlp_metric_utils.cc)
set_target_properties(opentelemetry_otlp_recordable PROPERTIES EXPORT_NAME
                                                               otlp_recordable)

target_include_directories(
  opentelemetry_otlp_recordable
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

set(OPENTELEMETRY_OTLP_TARGETS opentelemetry_otlp_recordable)
target_link_libraries(
  opentelemetry_otlp_recordable
  PUBLIC opentelemetry_trace opentelemetry_resources opentelemetry_proto)

if(WITH_LOGS_PREVIEW)
  target_link_libraries(opentelemetry_otlp_recordable PUBLIC opentelemetry_logs)
endif()
target_link_libraries(opentelemetry_otlp_recordable
                      PUBLIC opentelemetry_metrics)

if(WITH_OTLP_GRPC)
  find_package(gRPC REQUIRED)
  add_library(opentelemetry_exporter_otlp_grpc_client src/otlp_grpc_client.cc
                                                      src/otlp_grpc_utils.cc)
  set_target_properties(opentelemetry_exporter_otlp_grpc_client
                        PROPERTIES EXPORT_NAME otlp_grpc_client)
  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_client
    PUBLIC opentelemetry_sdk opentelemetry_ext opentelemetry_proto)

  target_link_libraries(opentelemetry_exporter_otlp_grpc_client
                        PRIVATE gRPC::grpc++)
  get_target_property(GRPC_INCLUDE_DIRECTORY gRPC::grpc++
                      INTERFACE_INCLUDE_DIRECTORIES)
  if(GRPC_INCLUDE_DIRECTORY)
    target_include_directories(
      opentelemetry_exporter_otlp_grpc_client
      PUBLIC "$<BUILD_INTERFACE:${GRPC_INCLUDE_DIRECTORY}>")
  endif()
  target_include_directories(
    opentelemetry_exporter_otlp_grpc_client
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  list(APPEND OPENTELEMETRY_OTLP_TARGETS
       opentelemetry_exporter_otlp_grpc_client)

  add_library(opentelemetry_exporter_otlp_grpc
              src/otlp_grpc_exporter.cc src/otlp_grpc_exporter_factory.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc
                        PROPERTIES EXPORT_NAME otlp_grpc_exporter)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_TARGETS opentelemetry_exporter_otlp_grpc)

  add_library(
    opentelemetry_exporter_otlp_grpc_log
    src/otlp_grpc_log_record_exporter.cc
    src/otlp_grpc_log_record_exporter_factory.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc_log
                        PROPERTIES EXPORT_NAME otlp_grpc_log_record_exporter)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_log
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_TARGETS opentelemetry_exporter_otlp_grpc_log)

  add_library(
    opentelemetry_exporter_otlp_grpc_metrics
    src/otlp_grpc_metric_exporter.cc src/otlp_grpc_metric_exporter_factory.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc_metrics
                        PROPERTIES EXPORT_NAME otlp_grpc_metrics_exporter)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_metrics
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_TARGETS
       opentelemetry_exporter_otlp_grpc_metrics)
endif()

if(WITH_OTLP_HTTP)
  add_library(opentelemetry_exporter_otlp_http_client src/otlp_http_client.cc)
  set_target_properties(opentelemetry_exporter_otlp_http_client
                        PROPERTIES EXPORT_NAME otlp_http_client)
  target_link_libraries(
    opentelemetry_exporter_otlp_http_client
    PUBLIC opentelemetry_sdk opentelemetry_proto opentelemetry_http_client_curl
           nlohmann_json::nlohmann_json)
  if(nlohmann_json_clone)
    add_dependencies(opentelemetry_exporter_otlp_http_client
                     nlohmann_json::nlohmann_json)
  endif()
  target_include_directories(
    opentelemetry_exporter_otlp_http_client
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  list(APPEND OPENTELEMETRY_OTLP_TARGETS
       opentelemetry_exporter_otlp_http_client)

  add_library(opentelemetry_exporter_otlp_http
              src/otlp_http_exporter.cc src/otlp_http_exporter_factory.cc)

  set_target_properties(opentelemetry_exporter_otlp_http
                        PROPERTIES EXPORT_NAME otlp_http_exporter)

  target_link_libraries(
    opentelemetry_exporter_otlp_http
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_http_client)

  list(APPEND OPENTELEMETRY_OTLP_TARGETS opentelemetry_exporter_otlp_http)

  if(WITH_LOGS_PREVIEW)
    add_library(
      opentelemetry_exporter_otlp_http_log
      src/otlp_http_log_record_exporter.cc
      src/otlp_http_log_record_exporter_factory.cc)

    set_target_properties(opentelemetry_exporter_otlp_http_log
                          PROPERTIES EXPORT_NAME otlp_http_log_record_exporter)

    target_link_libraries(
      opentelemetry_exporter_otlp_http_log
      PUBLIC opentelemetry_otlp_recordable
             opentelemetry_exporter_otlp_http_client)

    list(APPEND OPENTELEMETRY_OTLP_TARGETS opentelemetry_exporter_otlp_http_log)

  endif()
  add_library(
    opentelemetry_exporter_otlp_http_metric
    src/otlp_http_metric_exporter.cc src/otlp_http_metric_exporter_factory.cc)

  set_target_properties(opentelemetry_exporter_otlp_http_metric
                        PROPERTIES EXPORT_NAME otlp_http_metric_exporter)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_metric
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_http_client)

  list(APPEND OPENTELEMETRY_OTLP_TARGETS
       opentelemetry_exporter_otlp_http_metric)
endif()

install(
  TARGETS ${OPENTELEMETRY_OTLP_TARGETS}
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/exporters/otlp
  DESTINATION include/opentelemetry/exporters
  FILES_MATCHING
  PATTERN "*.h")

if(BUILD_TESTING)
  add_executable(otlp_recordable_test test/otlp_recordable_test.cc)
  target_link_libraries(otlp_recordable_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_otlp_recordable)
  gtest_add_tests(
    TARGET otlp_recordable_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_recordable_test)

  if(WITH_LOGS_PREVIEW)
    add_executable(otlp_log_recordable_test test/otlp_log_recordable_test.cc)
    target_link_libraries(
      otlp_log_recordable_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_otlp_recordable)
    gtest_add_tests(
      TARGET otlp_log_recordable_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_log_recordable_test)
  endif()

  add_executable(otlp_metrics_serialization_test
                 test/otlp_metrics_serialization_test.cc)
  target_link_libraries(otlp_metrics_serialization_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_otlp_recordable)
  gtest_add_tests(
    TARGET otlp_metrics_serialization_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_metrics_serialization_test)

  if(MSVC)
    # Explicitly specify that we consume GTest from shared library. The rest of
    # code logic below determines whether we link Release or Debug flavor of the
    # library. These flavors have different prefix on Windows, gmock and gmockd
    # respectively.
    add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
    if(GMOCK_LIB)
      # unset GMOCK_LIB to force find_library to redo the lookup, as the cached
      # entry could cause linking to incorrect flavor of gmock and leading to
      # runtime error.
      unset(GMOCK_LIB CACHE)
    endif()
  endif()
  if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(GMOCK_LIB gmockd PATH_SUFFIXES lib)
  else()
    find_library(GMOCK_LIB gmock PATH_SUFFIXES lib)
  endif()

  if(WITH_OTLP_GRPC)
    add_executable(otlp_grpc_exporter_test test/otlp_grpc_exporter_test.cc)
    target_link_libraries(
      otlp_grpc_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc)
    gtest_add_tests(
      TARGET otlp_grpc_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_exporter_test)

    add_executable(otlp_grpc_exporter_factory_test
                   test/otlp_grpc_exporter_factory_test.cc)
    target_link_libraries(
      otlp_grpc_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc)
    gtest_add_tests(
      TARGET otlp_grpc_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_exporter_factory_test)

    if(WITH_LOGS_PREVIEW)
      add_executable(otlp_grpc_log_record_exporter_test
                     test/otlp_grpc_log_record_exporter_test.cc)
      target_link_libraries(
        otlp_grpc_log_record_exporter_test ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
        opentelemetry_exporter_otlp_grpc_log opentelemetry_logs)
      gtest_add_tests(
        TARGET otlp_grpc_log_record_exporter_test
        TEST_PREFIX exporter.otlp.
        TEST_LIST otlp_grpc_log_record_exporter_test)

      add_executable(otlp_grpc_log_record_exporter_factory_test
                     test/otlp_grpc_log_record_exporter_factory_test.cc)
      target_link_libraries(
        otlp_grpc_log_record_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
        opentelemetry_exporter_otlp_grpc_log opentelemetry_logs)
      gtest_add_tests(
        TARGET otlp_grpc_log_record_exporter_factory_test
        TEST_PREFIX exporter.otlp.
        TEST_LIST otlp_grpc_log_record_exporter_factory_test)
    endif()

    add_executable(otlp_grpc_metric_exporter_factory_test
                   test/otlp_grpc_metric_exporter_factory_test.cc)
    target_link_libraries(
      otlp_grpc_metric_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
      opentelemetry_exporter_otlp_grpc_metrics opentelemetry_metrics)
    gtest_add_tests(
      TARGET otlp_grpc_metric_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_metric_exporter_factory_test)
  endif()

  if(WITH_OTLP_HTTP)
    add_executable(otlp_http_exporter_test test/otlp_http_exporter_test.cc)
    target_link_libraries(
      otlp_http_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB} opentelemetry_exporter_otlp_http
      opentelemetry_http_client_nosend)
    gtest_add_tests(
      TARGET otlp_http_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_http_exporter_test)

    add_executable(otlp_http_exporter_factory_test
                   test/otlp_http_exporter_factory_test.cc)
    target_link_libraries(
      otlp_http_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB} opentelemetry_exporter_otlp_http)
    gtest_add_tests(
      TARGET otlp_http_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_http_exporter_factory_test)

    if(WITH_LOGS_PREVIEW)
      add_executable(otlp_http_log_record_exporter_test
                     test/otlp_http_log_record_exporter_test.cc)
      target_link_libraries(
        otlp_http_log_record_exporter_test
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${GMOCK_LIB}
        opentelemetry_exporter_otlp_http_log
        opentelemetry_logs
        opentelemetry_http_client_nosend)
      gtest_add_tests(
        TARGET otlp_http_log_record_exporter_test
        TEST_PREFIX exporter.otlp.
        TEST_LIST otlp_http_log_record_exporter_test)

      add_executable(otlp_http_log_record_exporter_factory_test
                     test/otlp_http_log_record_exporter_factory_test.cc)
      target_link_libraries(
        otlp_http_log_record_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
        opentelemetry_exporter_otlp_http_log opentelemetry_logs)
      gtest_add_tests(
        TARGET otlp_http_log_record_exporter_factory_test
        TEST_PREFIX exporter.otlp.
        TEST_LIST otlp_http_log_record_exporter_factory_test)
    endif()

    add_executable(otlp_http_metric_exporter_test
                   test/otlp_http_metric_exporter_test.cc)
    target_link_libraries(
      otlp_http_metric_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_http_metric
      opentelemetry_metrics
      opentelemetry_http_client_nosend)
    gtest_add_tests(
      TARGET otlp_http_metric_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_http_metric_exporter_test)

    add_executable(otlp_http_metric_exporter_factory_test
                   test/otlp_http_metric_exporter_factory_test.cc)
    target_link_libraries(
      otlp_http_metric_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
      opentelemetry_exporter_otlp_http_metric opentelemetry_metrics)
    gtest_add_tests(
      TARGET otlp_http_metric_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_http_metric_exporter_factory_test)
  endif()
endif() # BUILD_TESTING
