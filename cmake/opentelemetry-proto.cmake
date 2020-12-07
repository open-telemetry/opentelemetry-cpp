set(PROTO_PATH "${CMAKE_SOURCE_DIR}/third_party/opentelemetry-proto")

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
set(TRACE_SERVICE_GRPC_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.cc"
)
set(TRACE_SERVICE_GRPC_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
)
set(LOGS_SERVICE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.pb.cc"
)
set(LOGS_SERVICE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
)
set(LOGS_SERVICE_GRPC_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.cc"
)
set(LOGS_SERVICE_GRPC_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"
)
set(METRICS_SERVICE_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.pb.cc"
)
set(METRICS_SERVICE_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
)
set(METRICS_SERVICE_GRPC_PB_CPP_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.cc"
)
set(METRICS_SERVICE_GRPC_PB_H_FILE
    "${GENERATED_PROTOBUF_PATH}/opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"
)

foreach(IMPORT_DIR ${PROTOBUF_IMPORT_DIRS})
  list(APPEND PROTOBUF_INCLUDE_FLAGS "-I${IMPORT_DIR}")
endforeach()

get_target_property(gRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin
                    IMPORTED_LOCATION_RELEASE)

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
    ${PROTOBUF_PROTOC_EXECUTABLE} ARGS "--proto_path=${PROTO_PATH}"
    ${PROTOBUF_INCLUDE_FLAGS} "--cpp_out=${GENERATED_PROTOBUF_PATH}"
    "--grpc_out=${GENERATED_PROTOBUF_PATH}"
    --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}" ${COMMON_PROTO}
    ${RESOURCE_PROTO} ${TRACE_PROTO} ${LOGS_PROTO} ${METRICS_PROTO}
    ${TRACE_SERVICE_PROTO} ${LOGS_SERVICE_PROTO} ${METRICS_SERVICE_PROTO})

include_directories("${GENERATED_PROTOBUF_PATH}")

add_library(
  opentelemetry_proto OBJECT
  ${COMMON_PB_CPP_FILE}
  ${RESOURCE_PB_CPP_FILE}
  ${TRACE_PB_CPP_FILE}
  ${LOGS_PB_CPP_FILE}
  ${METRICS_PB_CPP_FILE}
  ${TRACE_SERVICE_PB_CPP_FILE}
  ${TRACE_SERVICE_GRPC_PB_CPP_FILE}
  ${LOGS_SERVICE_PB_CPP_FILE}
  ${LOGS_SERVICE_GRPCPB_CPP_FILE}
  ${METRICS_SERVICE_PB_CPP_FILE}
  ${METRICS_SERVICE_GRPC_PB_CPP_FILE})

if(TARGET protobuf::libprotobuf)
  target_link_libraries(opentelemetry_proto
    PUBLIC protobuf::libprotobuf
  )
else() # cmake 3.8 or lower
  target_include_directories(opentelemetry_proto
    PUBLIC ${Protobuf_INCLUDE_DIRS}
  )
  target_include_directories(opentelemetry_proto
    INTERFACE ${Protobuf_LIBRARIES}
  )
endif()

if(BUILD_SHARED_LIBS)
  set_property(TARGET opentelemetry_proto PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
