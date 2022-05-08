if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opentelemetry-proto/.git)
  set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opentelemetry-proto")
  set(needs_proto_download FALSE)
else()
  if("${opentelemetry-proto}" STREQUAL "")
    set(opentelemetry-proto "v0.17.0")
  endif()
  include(ExternalProject)
  ExternalProject_Add(
    opentelemetry-proto
    GIT_REPOSITORY https://github.com/open-telemetry/opentelemetry-proto.git
    GIT_TAG "${opentelemetry-proto}"
    UPDATE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    TEST_AFTER_INSTALL 0
    DOWNLOAD_NO_PROGRESS 1
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1)
  ExternalProject_Get_Property(opentelemetry-proto INSTALL_DIR)
  set(PROTO_PATH "${INSTALL_DIR}/src/opentelemetry-proto")
  set(needs_proto_download TRUE)
endif()

include(${PROJECT_SOURCE_DIR}/cmake/proto-options-patch.cmake)

set(COMMON_PROTO "${PROTO_PATH}/opentelemetry/proto/common/v1/common.proto")
set(RESOURCE_PROTO
    "${PROTO_PATH}/opentelemetry/proto/resource/v1/resource.proto")
set(TRACE_PROTO "${PROTO_PATH}/opentelemetry/proto/trace/v1/trace.proto")
set(LOGS_PROTO "${PROTO_PATH}/opentelemetry/proto/logs/v1/logs.proto")
set(METRICS_PROTO "${PROTO_PATH}/opentelemetry/proto/metrics/v1/metrics.proto")

set(TRACE_SERVICE_PROTO
    "${PROTO_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.proto")
set(LOGS_SERVICE_PROTO
    "${PROTO_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.proto")
set(METRICS_SERVICE_PROTO
    "${PROTO_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.proto"
)

set(GENERATED_PROTOBUF_PATH
    "${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto")

file(MAKE_DIRECTORY "${GENERATED_PROTOBUF_PATH}")

set(COMMON_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/common/v1/common.pb.cc")
set(COMMON_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/common/v1/common.pb.h")
set(RESOURCE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/resource/v1/resource.pb.cc")
set(RESOURCE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/resource/v1/resource.pb.h")
set(TRACE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/trace/v1/trace.pb.cc")
set(TRACE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/trace/v1/trace.pb.h")
set(LOGS_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/logs/v1/logs.pb.cc")
set(LOGS_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/logs/v1/logs.pb.h")
set(METRICS_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/metrics/v1/metrics.pb.cc")
set(METRICS_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/metrics/v1/metrics.pb.h")

set(TRACE_SERVICE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.pb.cc"
)
set(TRACE_SERVICE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
)
if(WITH_OTLP_GRPC)
  set(TRACE_SERVICE_GRPC_PB_CPP_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.cc"
  )
  set(TRACE_SERVICE_GRPC_PB_H_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
  )
endif()
set(LOGS_SERVICE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.pb.cc"
)
set(LOGS_SERVICE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
)
if(WITH_OTLP_GRPC)
  set(LOGS_SERVICE_GRPC_PB_CPP_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.cc"
  )
  set(LOGS_SERVICE_GRPC_PB_H_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"
  )
endif()
set(METRICS_SERVICE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.pb.cc"
)
set(METRICS_SERVICE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
)
if(WITH_OTLP_GRPC)
  set(METRICS_SERVICE_GRPC_PB_CPP_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.cc"
  )
  set(METRICS_SERVICE_GRPC_PB_H_FILE
      "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"
  )
endif()

foreach(IMPORT_DIR ${PROTOBUF_IMPORT_DIRS})
  list(APPEND PROTOBUF_INCLUDE_FLAGS "-I${IMPORT_DIR}")
endforeach()

if(WITH_OTLP_GRPC)
  if(CMAKE_CROSSCOMPILING)
    find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
  else()
    if(TARGET gRPC::grpc_cpp_plugin)
      project_build_tools_get_imported_location(gRPC_CPP_PLUGIN_EXECUTABLE
                                                gRPC::grpc_cpp_plugin)
    else()
      find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
    endif()
  endif()
  message(STATUS "gRPC_CPP_PLUGIN_EXECUTABLE=${gRPC_CPP_PLUGIN_EXECUTABLE}")
endif()

if(WITH_OTLP_GRPC)
  add_custom_command(
    OUTPUT ${COMMON_PB_H_FILE}
           ${COMMON_PB_CPP_FILE}
           ${RESOURCE_PB_H_FILE}
           ${RESOURCE_PB_CPP_FILE}
           ${TRACE_PB_H_FILE}
           ${TRACE_PB_CPP_FILE}
           ${LOGS_PB_H_FILE}
           ${LOGS_PB_CPP_FILE}
           ${METRICS_PB_H_FILE}
           ${METRICS_PB_CPP_FILE}
           ${TRACE_SERVICE_PB_H_FILE}
           ${TRACE_SERVICE_PB_CPP_FILE}
           ${TRACE_SERVICE_GRPC_PB_H_FILE}
           ${TRACE_SERVICE_GRPC_PB_CPP_FILE}
           ${LOGS_SERVICE_PB_H_FILE}
           ${LOGS_SERVICE_PB_CPP_FILE}
           ${LOGS_SERVICE_GRPC_PB_H_FILE}
           ${LOGS_SERVICE_GRPC_PB_CPP_FILE}
           ${METRICS_SERVICE_PB_H_FILE}
           ${METRICS_SERVICE_PB_CPP_FILE}
           ${METRICS_SERVICE_GRPC_PB_H_FILE}
           ${METRICS_SERVICE_GRPC_PB_CPP_FILE}
    COMMAND
      ${PROTOBUF_PROTOC_EXECUTABLE} ARGS "--experimental_allow_proto3_optional"
      "--proto_path=${PROTO_PATH}" ${PROTOBUF_INCLUDE_FLAGS}
      "--cpp_out=${GENERATED_PROTOBUF_PATH}"
      "--grpc_out=generate_mock_code=true:${GENERATED_PROTOBUF_PATH}"
      --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}" ${COMMON_PROTO}
      ${RESOURCE_PROTO} ${TRACE_PROTO} ${LOGS_PROTO} ${METRICS_PROTO}
      ${TRACE_SERVICE_PROTO} ${LOGS_SERVICE_PROTO} ${METRICS_SERVICE_PROTO})
else()
  add_custom_command(
    OUTPUT ${COMMON_PB_H_FILE}
           ${COMMON_PB_CPP_FILE}
           ${RESOURCE_PB_H_FILE}
           ${RESOURCE_PB_CPP_FILE}
           ${TRACE_PB_H_FILE}
           ${TRACE_PB_CPP_FILE}
           ${LOGS_PB_H_FILE}
           ${LOGS_PB_CPP_FILE}
           ${METRICS_PB_H_FILE}
           ${METRICS_PB_CPP_FILE}
           ${TRACE_SERVICE_PB_H_FILE}
           ${TRACE_SERVICE_PB_CPP_FILE}
           ${LOGS_SERVICE_PB_H_FILE}
           ${LOGS_SERVICE_PB_CPP_FILE}
           ${METRICS_SERVICE_PB_H_FILE}
           ${METRICS_SERVICE_PB_CPP_FILE}
    COMMAND
      ${PROTOBUF_PROTOC_EXECUTABLE} ARGS "--experimental_allow_proto3_optional"
      "--proto_path=${PROTO_PATH}" ${PROTOBUF_INCLUDE_FLAGS}
      "--cpp_out=${GENERATED_PROTOBUF_PATH}" ${COMMON_PROTO} ${RESOURCE_PROTO}
      ${TRACE_PROTO} ${LOGS_PROTO} ${METRICS_PROTO} ${TRACE_SERVICE_PROTO}
      ${LOGS_SERVICE_PROTO} ${METRICS_SERVICE_PROTO})
endif()

include_directories("${GENERATED_PROTOBUF_PATH}")

if(WITH_OTLP_GRPC)
  add_library(
    opentelemetry_proto STATIC
    ${COMMON_PB_CPP_FILE}
    ${RESOURCE_PB_CPP_FILE}
    ${TRACE_PB_CPP_FILE}
    ${LOGS_PB_CPP_FILE}
    ${METRICS_PB_CPP_FILE}
    ${TRACE_SERVICE_PB_CPP_FILE}
    ${TRACE_SERVICE_GRPC_PB_CPP_FILE}
    ${LOGS_SERVICE_PB_CPP_FILE}
    ${LOGS_SERVICE_GRPC_PB_CPP_FILE}
    ${METRICS_SERVICE_PB_CPP_FILE}
    ${METRICS_SERVICE_GRPC_PB_CPP_FILE})
else()
  add_library(
    opentelemetry_proto STATIC
    ${COMMON_PB_CPP_FILE}
    ${RESOURCE_PB_CPP_FILE}
    ${TRACE_PB_CPP_FILE}
    ${LOGS_PB_CPP_FILE}
    ${METRICS_PB_CPP_FILE}
    ${TRACE_SERVICE_PB_CPP_FILE}
    ${LOGS_SERVICE_PB_CPP_FILE}
    ${METRICS_SERVICE_PB_CPP_FILE})
endif()

if(needs_proto_download)
  add_dependencies(opentelemetry_proto opentelemetry-proto)
endif()
set_target_properties(opentelemetry_proto PROPERTIES EXPORT_NAME proto)
patch_protobuf_targets(opentelemetry_proto)

install(
  TARGETS opentelemetry_proto
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY ${GENERATED_PROTOBUF_PATH}/opentelemetry
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h")

if(TARGET protobuf::libprotobuf)
  target_link_libraries(opentelemetry_proto PUBLIC protobuf::libprotobuf)
else() # cmake 3.8 or lower
  target_include_directories(opentelemetry_proto
                             PUBLIC ${Protobuf_INCLUDE_DIRS})
  target_link_libraries(opentelemetry_proto INTERFACE ${Protobuf_LIBRARIES})
endif()

if(BUILD_SHARED_LIBS)
  set_property(TARGET opentelemetry_proto PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
