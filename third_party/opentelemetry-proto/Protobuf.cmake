set(PROTO_PATH "${CMAKE_SOURCE_DIR}/third_party/opentelemetry-proto")

set(COMMON_PROTO "${PROTO_PATH}/opentelemetry/proto/common/v1/common.proto")
set(RESOURCE_PROTO "${PROTO_PATH}/opentelemetry/proto/resource/v1/resource.proto")
set(TRACE_PROTO "${PROTO_PATH}/opentelemetry/proto/trace/v1/trace.proto")

set(GENERATED_PROTOBUF_PATH "${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto")

file(MAKE_DIRECTORY "${GENERATED_PROTOBUF_PATH}")

set(COMMON_PB_CPP_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/common/v1/common.pb.cc")
set(COMMON_PB_H_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/common/v1/common.pb.h")
set(RESOURCE_PB_CPP_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/resource/v1/resource.pb.cc")
set(RESOURCE_PB_H_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/resource/v1/resource.pb.h")
set(TRACE_PB_CPP_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/trace/v1/trace.pb.cc")
set(TRACE_PB_H_FILE "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/trace/v1/trace.pb.h")

foreach(IMPORT_DIR ${PROTOBUF_IMPORT_DIRS})
  list(APPEND PROTOBUF_INCLUDE_FLAGS "-I${IMPORT_DIR}")
endforeach()

add_custom_command(
  OUTPUT
    ${COMMON_PB_H_FILE}
    ${COMMON_PB_CPP_FILE}
    ${RESOURCE_PB_H_FILE}
    ${RESOURCE_PB_CPP_FILE}
    ${TRACE_PB_H_FILE}
    ${TRACE_PB_CPP_FILE}
  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
  ARGS
    "--proto_path=${PROTO_PATH}"
    ${PROTOBUF_INCLUDE_FLAGS}
    "--cpp_out=${GENERATED_PROTOBUF_PATH}"
    ${COMMON_PROTO}
    ${RESOURCE_PROTO}
    ${TRACE_PROTO}
)

include_directories(SYSTEM "${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto")

add_library(opentelemetry_proto OBJECT
    ${COMMON_PB_CPP_FILE}
    ${RESOURCE_PB_CPP_FILE}
    ${TRACE_PB_CPP_FILE})
if (BUILD_SHARED_LIBS)
  set_property(TARGET opentelemetry_proto PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
