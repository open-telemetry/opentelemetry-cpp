# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(protobuf_lib_type STREQUAL "STATIC_LIBRARY")
  set(OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE STATIC)
else()
  set(OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE)
endif()

#
# opentelemetry_otlp_recordable
#

add_library(
  opentelemetry_otlp_recordable
  ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE}
  src/otlp_environment.cc
  src/otlp_log_recordable.cc
  src/otlp_recordable.cc
  src/otlp_populate_attribute_utils.cc
  src/otlp_recordable_utils.cc
  src/otlp_metric_utils.cc)
set_target_properties(opentelemetry_otlp_recordable PROPERTIES EXPORT_NAME
                                                               otlp_recordable)
set_target_version(opentelemetry_otlp_recordable)

target_include_directories(
  opentelemetry_otlp_recordable
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

set(OPENTELEMETRY_OTLP_TARGETS opentelemetry_otlp_recordable)

target_link_libraries(opentelemetry_otlp_recordable PUBLIC opentelemetry_logs)
target_link_libraries(opentelemetry_otlp_recordable
                      PUBLIC opentelemetry_metrics)

#
# opentelemetry_exporter_otlp_builder_utils
#

add_library(opentelemetry_exporter_otlp_builder_utils
            ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_builder_utils.cc)

set_target_properties(opentelemetry_exporter_otlp_builder_utils
                      PROPERTIES EXPORT_NAME otlp_builder_utils)
set_target_version(opentelemetry_exporter_otlp_builder_utils)

target_include_directories(
  opentelemetry_exporter_otlp_builder_utils
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(opentelemetry_exporter_otlp_builder_utils
                      PUBLIC opentelemetry_otlp_recordable)

if(WITH_OTLP_GRPC)
  if(NOT DEFINED grpc_lib_type)
    message(
      FATAL_ERROR "cmake/opentelemetry-proto.cmake should be included first")
  endif()
  if(grpc_lib_type STREQUAL "STATIC_LIBRARY" OR protobuf_lib_type STREQUAL
                                                "STATIC_LIBRARY")
    set(OPENTELEMETRY_OTLP_GRPC_CLIENT_LIB_TYPE STATIC)
  else()
    set(OPENTELEMETRY_OTLP_GRPC_CLIENT_LIB_TYPE)
  endif()

  #
  # opentelemetry_exporter_otlp_grpc_client
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_client
    ${OPENTELEMETRY_OTLP_GRPC_CLIENT_LIB_TYPE} src/otlp_grpc_client.cc
    src/otlp_grpc_client_factory.cc src/otlp_grpc_utils.cc)
  set_target_properties(opentelemetry_exporter_otlp_grpc_client
                        PROPERTIES EXPORT_NAME otlp_grpc_client)
  set_target_version(opentelemetry_exporter_otlp_grpc_client)

  # Because opentelemetry_proto_grpc depends on gRPC, and gRPC can only be
  # linked in one dynamic library or executable (otherwise, there may be
  # conflicts with some global variables in certain versions of gRPC), we have
  # switched the link for opentelemetry_exporter_otlp_grpc_client into all
  # targets that depend on opentelemetry_proto_grpc.
  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_client
    PUBLIC opentelemetry_sdk opentelemetry_common
           # gRPC::grpc++ must be linked before opentelemetry_proto_grpc.
           "$<BUILD_INTERFACE:opentelemetry_proto_grpc>"
    PRIVATE "$<INSTALL_INTERFACE:opentelemetry_proto_grpc>" gRPC::grpc++
            opentelemetry_ext)

  get_target_property(GRPC_INCLUDE_DIRECTORY gRPC::grpc++
                      INTERFACE_INCLUDE_DIRECTORIES)
  if(GRPC_INCLUDE_DIRECTORY)
    target_include_directories(
      opentelemetry_exporter_otlp_grpc_client BEFORE
      PUBLIC "$<BUILD_INTERFACE:${GRPC_INCLUDE_DIRECTORY}>")
  endif()
  target_include_directories(
    opentelemetry_exporter_otlp_grpc_client
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  list(APPEND OPENTELEMETRY_OTLP_GRPC_TARGETS
       opentelemetry_exporter_otlp_grpc_client opentelemetry_proto_grpc)

  #
  # opentelemetry_exporter_otlp_grpc
  #

  add_library(
    opentelemetry_exporter_otlp_grpc
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_exporter.cc
    src/otlp_grpc_exporter_factory.cc src/otlp_grpc_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc
                        PROPERTIES EXPORT_NAME otlp_grpc_exporter)
  set_target_version(opentelemetry_exporter_otlp_grpc)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_TARGETS opentelemetry_exporter_otlp_grpc)

  #
  # opentelemetry_exporter_otlp_grpc_builder
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_span_builder.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc_builder
                        PROPERTIES EXPORT_NAME otlp_grpc_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_grpc_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_builder
    PUBLIC opentelemetry_exporter_otlp_grpc
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_BUILDER_TARGETS
       opentelemetry_exporter_otlp_grpc_builder)

  #
  # opentelemetry_exporter_otlp_grpc_log
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_log
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_log_record_exporter.cc
    src/otlp_grpc_log_record_exporter_factory.cc
    src/otlp_grpc_log_record_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc_log
                        PROPERTIES EXPORT_NAME otlp_grpc_log_record_exporter)
  set_target_version(opentelemetry_exporter_otlp_grpc_log)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_log
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_TARGETS
       opentelemetry_exporter_otlp_grpc_log)

  #
  # opentelemetry_exporter_otlp_grpc_log_builder
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_log_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_log_record_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_grpc_log_builder
    PROPERTIES EXPORT_NAME otlp_grpc_log_record_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_grpc_log_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_log_builder
    PUBLIC opentelemetry_exporter_otlp_grpc_log
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_BUILDER_TARGETS
       opentelemetry_exporter_otlp_grpc_log_builder)

  #
  # opentelemetry_exporter_otlp_grpc_metrics
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_metrics
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_metric_exporter.cc
    src/otlp_grpc_metric_exporter_factory.cc
    src/otlp_grpc_metric_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_grpc_metrics
                        PROPERTIES EXPORT_NAME otlp_grpc_metrics_exporter)
  set_target_version(opentelemetry_exporter_otlp_grpc_metrics)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_metrics
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_grpc_client)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_TARGETS
       opentelemetry_exporter_otlp_grpc_metrics)

  #
  # opentelemetry_exporter_otlp_grpc_metric_builder
  #

  add_library(
    opentelemetry_exporter_otlp_grpc_metric_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_grpc_push_metric_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_grpc_metric_builder
    PROPERTIES EXPORT_NAME otlp_grpc_metric_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_grpc_metric_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_grpc_metric_builder
    PUBLIC opentelemetry_exporter_otlp_grpc_metrics
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_GRPC_BUILDER_TARGETS
       opentelemetry_exporter_otlp_grpc_metric_builder)

endif()

if(WITH_OTLP_HTTP)

  #
  # opentelemetry_exporter_otlp_http_client
  #

  add_library(
    opentelemetry_exporter_otlp_http_client
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http.cc
    src/otlp_http_client.cc)
  set_target_properties(opentelemetry_exporter_otlp_http_client
                        PROPERTIES EXPORT_NAME otlp_http_client)
  set_target_version(opentelemetry_exporter_otlp_http_client)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_client
    PUBLIC opentelemetry_sdk opentelemetry_ext
    # Links flags of opentelemetry_http_client_curl should be public when
    # building internal components
    PRIVATE opentelemetry_proto
            "$<BUILD_INTERFACE:opentelemetry_http_client_curl>"
            "$<BUILD_INTERFACE:nlohmann_json::nlohmann_json>"
            "$<INSTALL_INTERFACE:opentelemetry_http_client_curl>")

  target_include_directories(
    opentelemetry_exporter_otlp_http_client
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  list(APPEND OPENTELEMETRY_OTLP_HTTP_TARGETS
       opentelemetry_exporter_otlp_http_client)

  #
  # opentelemetry_exporter_otlp_http
  #

  add_library(
    opentelemetry_exporter_otlp_http
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_exporter.cc
    src/otlp_http_exporter_factory.cc src/otlp_http_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_http
                        PROPERTIES EXPORT_NAME otlp_http_exporter)
  set_target_version(opentelemetry_exporter_otlp_http)

  target_link_libraries(
    opentelemetry_exporter_otlp_http
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_http_client)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_TARGETS opentelemetry_exporter_otlp_http)

  #
  # opentelemetry_exporter_otlp_http_builder
  #

  add_library(
    opentelemetry_exporter_otlp_http_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_span_builder.cc)

  set_target_properties(opentelemetry_exporter_otlp_http_builder
                        PROPERTIES EXPORT_NAME otlp_http_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_http_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_builder
    PUBLIC opentelemetry_exporter_otlp_http
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_BUILDER_TARGETS
       opentelemetry_exporter_otlp_http_builder)

  #
  # opentelemetry_exporter_otlp_http_log
  #

  add_library(
    opentelemetry_exporter_otlp_http_log
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_log_record_exporter.cc
    src/otlp_http_log_record_exporter_factory.cc
    src/otlp_http_log_record_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_http_log
                        PROPERTIES EXPORT_NAME otlp_http_log_record_exporter)
  set_target_version(opentelemetry_exporter_otlp_http_log)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_log
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_http_client)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_TARGETS
       opentelemetry_exporter_otlp_http_log)

  #
  # opentelemetry_exporter_otlp_http_log_builder
  #

  add_library(
    opentelemetry_exporter_otlp_http_log_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_log_record_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_http_log_builder
    PROPERTIES EXPORT_NAME otlp_http_log_record_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_http_log_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_log_builder
    PUBLIC opentelemetry_exporter_otlp_http_log
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_BUILDER_TARGETS
       opentelemetry_exporter_otlp_http_log_builder)

  #
  # opentelemetry_exporter_otlp_http_metric
  #

  add_library(
    opentelemetry_exporter_otlp_http_metric
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_metric_exporter.cc
    src/otlp_http_metric_exporter_factory.cc
    src/otlp_http_metric_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_http_metric
                        PROPERTIES EXPORT_NAME otlp_http_metric_exporter)
  set_target_version(opentelemetry_exporter_otlp_http_metric)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_metric
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_http_client)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_TARGETS
       opentelemetry_exporter_otlp_http_metric)

  #
  # opentelemetry_exporter_otlp_http_metric_builder
  #

  add_library(
    opentelemetry_exporter_otlp_http_metric_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_http_push_metric_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_http_metric_builder
    PROPERTIES EXPORT_NAME otlp_http_metric_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_http_metric_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_http_metric_builder
    PUBLIC opentelemetry_exporter_otlp_http_metric
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_HTTP_BUILDER_TARGETS
       opentelemetry_exporter_otlp_http_metric_builder)

endif()

if(WITH_OTLP_FILE)

  #
  # opentelemetry_exporter_otlp_file_client
  #

  add_library(opentelemetry_exporter_otlp_file_client
              ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_client.cc)
  set_target_properties(opentelemetry_exporter_otlp_file_client
                        PROPERTIES EXPORT_NAME otlp_file_client)
  set_target_version(opentelemetry_exporter_otlp_file_client)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_client
    PUBLIC opentelemetry_sdk opentelemetry_common
    PRIVATE opentelemetry_proto $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>)

  target_include_directories(
    opentelemetry_exporter_otlp_file_client
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  list(APPEND OPENTELEMETRY_OTLP_FILE_TARGETS
       opentelemetry_exporter_otlp_file_client)

  #
  # opentelemetry_exporter_otlp_file
  #

  add_library(
    opentelemetry_exporter_otlp_file
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_exporter.cc
    src/otlp_file_exporter_factory.cc src/otlp_file_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_file
                        PROPERTIES EXPORT_NAME otlp_file_exporter)
  set_target_version(opentelemetry_exporter_otlp_file)

  target_link_libraries(
    opentelemetry_exporter_otlp_file
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_file_client)

  list(APPEND OPENTELEMETRY_OTLP_FILE_TARGETS opentelemetry_exporter_otlp_file)

  #
  # opentelemetry_exporter_otlp_file_builder
  #

  add_library(
    opentelemetry_exporter_otlp_file_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_span_builder.cc)

  set_target_properties(opentelemetry_exporter_otlp_file_builder
                        PROPERTIES EXPORT_NAME otlp_file_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_file_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_builder
    PUBLIC opentelemetry_exporter_otlp_file
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_FILE_BUILDER_TARGETS
       opentelemetry_exporter_otlp_file_builder)

  #
  # opentelemetry_exporter_otlp_file_log
  #

  add_library(
    opentelemetry_exporter_otlp_file_log
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_log_record_exporter.cc
    src/otlp_file_log_record_exporter_factory.cc
    src/otlp_file_log_record_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_file_log
                        PROPERTIES EXPORT_NAME otlp_file_log_record_exporter)
  set_target_version(opentelemetry_exporter_otlp_file_log)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_log
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_file_client)

  list(APPEND OPENTELEMETRY_OTLP_FILE_TARGETS
       opentelemetry_exporter_otlp_file_log)

  #
  # opentelemetry_exporter_otlp_file_log_builder
  #

  add_library(
    opentelemetry_exporter_otlp_file_log_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_log_record_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_file_log_builder
    PROPERTIES EXPORT_NAME otlp_file_log_record_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_file_log_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_log_builder
    PUBLIC opentelemetry_exporter_otlp_file_log
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_FILE_BUILDER_TARGETS
       opentelemetry_exporter_otlp_file_log_builder)

  #
  # opentelemetry_exporter_otlp_file_metric
  #

  add_library(
    opentelemetry_exporter_otlp_file_metric
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_metric_exporter.cc
    src/otlp_file_metric_exporter_factory.cc
    src/otlp_file_metric_exporter_options.cc)

  set_target_properties(opentelemetry_exporter_otlp_file_metric
                        PROPERTIES EXPORT_NAME otlp_file_metric_exporter)
  set_target_version(opentelemetry_exporter_otlp_file_metric)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_metric
    PUBLIC opentelemetry_otlp_recordable
           opentelemetry_exporter_otlp_file_client)

  list(APPEND OPENTELEMETRY_OTLP_FILE_TARGETS
       opentelemetry_exporter_otlp_file_metric)

  #
  # opentelemetry_exporter_otlp_file_metric_builder
  #

  add_library(
    opentelemetry_exporter_otlp_file_metric_builder
    ${OPENTELEMETRY_OTLP_TARGETS_LIB_TYPE} src/otlp_file_push_metric_builder.cc)

  set_target_properties(
    opentelemetry_exporter_otlp_file_metric_builder
    PROPERTIES EXPORT_NAME otlp_file_metric_exporter_builder)
  set_target_version(opentelemetry_exporter_otlp_file_metric_builder)

  target_link_libraries(
    opentelemetry_exporter_otlp_file_metric_builder
    PUBLIC opentelemetry_exporter_otlp_file_metric
           opentelemetry_exporter_otlp_builder_utils)

  list(APPEND OPENTELEMETRY_OTLP_FILE_BUILDER_TARGETS
       opentelemetry_exporter_otlp_file_metric_builder)

endif()

target_link_libraries(
  opentelemetry_otlp_recordable
  PUBLIC opentelemetry_trace opentelemetry_resources opentelemetry_proto)

otel_add_component(
  COMPONENT
  exporters_otlp_common
  TARGETS
  opentelemetry_otlp_recordable
  opentelemetry_proto
  FILES_DIRECTORY
  "include/opentelemetry/exporters/otlp"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "*.h"
  PATTERN
  "otlp_builder_utils*.h"
  EXCLUDE
  PATTERN
  "otlp_http*.h"
  EXCLUDE
  PATTERN
  "otlp_grpc*.h"
  EXCLUDE
  PATTERN
  "otlp_file*.h"
  EXCLUDE)

otel_add_component(
  COMPONENT
  exporter_otlp_builder_utils
  TARGETS
  opentelemetry_exporter_otlp_builder_utils
  FILES_DIRECTORY
  "include/opentelemetry/exporters/otlp"
  FILES_DESTINATION
  "include/opentelemetry/exporters"
  FILES_MATCHING
  PATTERN
  "otlp_builder_utils.h")

if(WITH_OTLP_GRPC)
  otel_add_component(
    COMPONENT
    exporters_otlp_grpc
    TARGETS
    ${OPENTELEMETRY_OTLP_GRPC_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_grpc*.h"
    PATTERN
    "otlp_grpc_span_builder.h"
    EXCLUDE
    PATTERN
    "otlp_grpc_log_record_builder.h"
    EXCLUDE
    PATTERN
    "otlp_grpc_push_metric_builder.h"
    EXCLUDE)

  otel_add_component(
    COMPONENT
    exporters_otlp_grpc_builder
    TARGETS
    ${OPENTELEMETRY_OTLP_GRPC_BUILDER_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_grpc_span_builder.h"
    PATTERN
    "otlp_grpc_log_record_builder.h"
    PATTERN
    "otlp_grpc_push_metric_builder.h")

endif()

if(WITH_OTLP_HTTP)
  otel_add_component(
    COMPONENT
    exporters_otlp_http
    TARGETS
    ${OPENTELEMETRY_OTLP_HTTP_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_http*.h"
    PATTERN
    "otlp_http_span_builder.h"
    EXCLUDE
    PATTERN
    "otlp_http_log_record_builder.h"
    EXCLUDE
    PATTERN
    "otlp_http_push_metric_builder.h"
    EXCLUDE)

  otel_add_component(
    COMPONENT
    exporters_otlp_http_builder
    TARGETS
    ${OPENTELEMETRY_OTLP_HTTP_BUILDER_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_http_span_builder.h"
    PATTERN
    "otlp_http_log_record_builder.h"
    PATTERN
    "otlp_http_push_metric_builder.h")

endif()

if(WITH_OTLP_FILE)
  otel_add_component(
    COMPONENT
    exporters_otlp_file
    TARGETS
    ${OPENTELEMETRY_OTLP_FILE_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_file*.h"
    PATTERN
    "otlp_file_span_builder.h"
    EXCLUDE
    PATTERN
    "otlp_file_log_record_builder.h"
    EXCLUDE
    PATTERN
    "otlp_file_push_metric_builder.h"
    EXCLUDE)

  otel_add_component(
    COMPONENT
    exporters_otlp_file_builder
    TARGETS
    ${OPENTELEMETRY_OTLP_FILE_BUILDER_TARGETS}
    FILES_DIRECTORY
    "include/opentelemetry/exporters/otlp"
    FILES_DESTINATION
    "include/opentelemetry/exporters"
    FILES_MATCHING
    PATTERN
    "otlp_file_span_builder.h"
    PATTERN
    "otlp_file_log_record_builder.h"
    PATTERN
    "otlp_file_push_metric_builder.h")

endif()

if(BUILD_TESTING)
  add_executable(otlp_recordable_test test/otlp_recordable_test.cc)
  target_link_libraries(
    otlp_recordable_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_otlp_recordable protobuf::libprotobuf)
  gtest_add_tests(
    TARGET otlp_recordable_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_recordable_test)

  add_executable(otlp_log_recordable_test test/otlp_log_recordable_test.cc)
  target_link_libraries(otlp_log_recordable_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} opentelemetry_otlp_recordable)
  gtest_add_tests(
    TARGET otlp_log_recordable_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_log_recordable_test)

  add_executable(otlp_metrics_serialization_test
                 test/otlp_metrics_serialization_test.cc)
  target_link_libraries(
    otlp_metrics_serialization_test ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} opentelemetry_otlp_recordable
    protobuf::libprotobuf)
  gtest_add_tests(
    TARGET otlp_metrics_serialization_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_metrics_serialization_test)

  if(WITH_OTLP_GRPC)
    add_executable(otlp_grpc_exporter_test test/otlp_grpc_exporter_test.cc)
    target_link_libraries(
      otlp_grpc_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc)
    gtest_add_tests(
      TARGET otlp_grpc_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_exporter_test)

    add_executable(otlp_grpc_target_test test/otlp_grpc_target_test.cc)
    target_link_libraries(
      otlp_grpc_target_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc)
    gtest_add_tests(
      TARGET otlp_grpc_target_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_target_test)

    add_executable(otlp_grpc_exporter_factory_test
                   test/otlp_grpc_exporter_factory_test.cc)
    target_link_libraries(
      otlp_grpc_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc)
    gtest_add_tests(
      TARGET otlp_grpc_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_exporter_factory_test)

    add_executable(otlp_grpc_log_record_exporter_test
                   test/otlp_grpc_log_record_exporter_test.cc)
    target_link_libraries(
      otlp_grpc_log_record_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_grpc
      opentelemetry_exporter_otlp_grpc_log
      opentelemetry_trace
      opentelemetry_logs)
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

    add_executable(otlp_grpc_metric_exporter_test
                   test/otlp_grpc_metric_exporter_test.cc)
    target_link_libraries(
      otlp_grpc_metric_exporter_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB} opentelemetry_exporter_otlp_grpc
      opentelemetry_exporter_otlp_grpc_metrics)
    gtest_add_tests(
      TARGET otlp_grpc_metric_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_grpc_metric_exporter_test)

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
      otlp_http_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_http
      opentelemetry_http_client_nosend
      nlohmann_json::nlohmann_json
      protobuf::libprotobuf)
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

    add_executable(otlp_http_log_record_exporter_test
                   test/otlp_http_log_record_exporter_test.cc)
    target_link_libraries(
      otlp_http_log_record_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_http_log
      opentelemetry_logs
      opentelemetry_http_client_nosend
      nlohmann_json::nlohmann_json)
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

    add_executable(otlp_http_metric_exporter_test
                   test/otlp_http_metric_exporter_test.cc)
    target_link_libraries(
      otlp_http_metric_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_http_metric
      opentelemetry_metrics
      opentelemetry_http_client_nosend
      nlohmann_json::nlohmann_json
      protobuf::libprotobuf)
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

  if(WITH_OTLP_FILE)
    add_executable(otlp_file_client_test test/otlp_file_client_test.cc)
    target_link_libraries(
      otlp_file_client_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file
      opentelemetry_otlp_recordable
      nlohmann_json::nlohmann_json)
    gtest_add_tests(
      TARGET otlp_file_client_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_client_test)

    add_executable(otlp_file_exporter_test test/otlp_file_exporter_test.cc)
    target_link_libraries(
      otlp_file_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file
      nlohmann_json::nlohmann_json
      protobuf::libprotobuf)
    gtest_add_tests(
      TARGET otlp_file_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_exporter_test)

    add_executable(otlp_file_exporter_factory_test
                   test/otlp_file_exporter_factory_test.cc)
    target_link_libraries(
      otlp_file_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB} opentelemetry_exporter_otlp_file)
    gtest_add_tests(
      TARGET otlp_file_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_exporter_factory_test)

    add_executable(otlp_file_log_record_exporter_test
                   test/otlp_file_log_record_exporter_test.cc)
    target_link_libraries(
      otlp_file_log_record_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file_log
      opentelemetry_logs
      nlohmann_json::nlohmann_json)
    gtest_add_tests(
      TARGET otlp_file_log_record_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_log_record_exporter_test)

    add_executable(otlp_file_log_record_exporter_factory_test
                   test/otlp_file_log_record_exporter_factory_test.cc)
    target_link_libraries(
      otlp_file_log_record_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file_log opentelemetry_logs)
    gtest_add_tests(
      TARGET otlp_file_log_record_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_log_record_exporter_factory_test)

    add_executable(otlp_file_metric_exporter_test
                   test/otlp_file_metric_exporter_test.cc)
    target_link_libraries(
      otlp_file_metric_exporter_test
      ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file_metric
      opentelemetry_metrics
      nlohmann_json::nlohmann_json
      protobuf::libprotobuf)
    gtest_add_tests(
      TARGET otlp_file_metric_exporter_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_metric_exporter_test)

    add_executable(otlp_file_metric_exporter_factory_test
                   test/otlp_file_metric_exporter_factory_test.cc)
    target_link_libraries(
      otlp_file_metric_exporter_factory_test ${GTEST_BOTH_LIBRARIES}
      ${CMAKE_THREAD_LIBS_INIT} ${GMOCK_LIB}
      opentelemetry_exporter_otlp_file_metric opentelemetry_metrics)
    gtest_add_tests(
      TARGET otlp_file_metric_exporter_factory_test
      TEST_PREFIX exporter.otlp.
      TEST_LIST otlp_file_metric_exporter_factory_test)
  endif()
endif() # BUILD_TESTING
