macro(check_append_cxx_compiler_flag OUTPUT_VAR)
  foreach(CHECK_FLAG ${ARGN})
    check_cxx_compiler_flag(${CHECK_FLAG}
                            "check_cxx_compiler_flag_${CHECK_FLAG}")
    if(check_cxx_compiler_flag_${CHECK_FLAG})
      list(APPEND ${OUTPUT_VAR} ${CHECK_FLAG})
    endif()
  endforeach()
endmacro()

if(NOT PATCH_PROTOBUF_SOURCES_OPTIONS_SET)
  if(MSVC)
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS /wd4244 /wd4251 /wd4267 /wd4309)

    if(MSVC_VERSION GREATER_EQUAL 1922)
      # see
      # https://docs.microsoft.com/en-us/cpp/overview/cpp-conformance-improvements?view=vs-2019#improvements_162
      # for detail
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd5054)
    endif()

    if(MSVC_VERSION GREATER_EQUAL 1925)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4996)
    endif()

    if(MSVC_VERSION LESS 1910)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4800)
    endif()
  else()
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    include(CheckCXXCompilerFlag)
    check_append_cxx_compiler_flag(
      PATCH_PROTOBUF_SOURCES_OPTIONS -Wno-type-limits
      -Wno-deprecated-declarations -Wno-unused-parameter)
  endif()
  set(PATCH_PROTOBUF_SOURCES_OPTIONS_SET TRUE)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS
        ${PATCH_PROTOBUF_SOURCES_OPTIONS}
        CACHE INTERNAL
              "Options to disable warning of generated protobuf sources" FORCE)
  endif()
endif()

function(patch_protobuf_sources)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_SRC ${ARGN})
      unset(PROTO_SRC_OPTIONS)
      get_source_file_property(PROTO_SRC_OPTIONS ${PROTO_SRC} COMPILE_OPTIONS)
      if(PROTO_SRC_OPTIONS)
        list(APPEND PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_source_files_properties(
        ${PROTO_SRC} PROPERTIES COMPILE_OPTIONS "${PROTO_SRC_OPTIONS}")
    endforeach()
    unset(PROTO_SRC)
    unset(PROTO_SRC_OPTIONS)
  endif()
endfunction()

function(patch_protobuf_targets)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_TARGET ${ARGN})
      unset(PROTO_TARGET_OPTIONS)
      get_target_property(PROTO_TARGET_OPTIONS ${PROTO_TARGET} COMPILE_OPTIONS)
      if(PROTO_TARGET_OPTIONS)
        list(APPEND PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_target_properties(
        ${PROTO_TARGET} PROPERTIES COMPILE_OPTIONS "${PROTO_TARGET_OPTIONS}")
    endforeach()
    unset(PROTO_TARGET)
    unset(PROTO_TARGET_OPTIONS)
  endif()
endfunction()

set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opentelemetry-proto")

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

set(gRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

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
    "--grpc_out=generate_mock_code=true:${GENERATED_PROTOBUF_PATH}"
    --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}" ${COMMON_PROTO}
    ${RESOURCE_PROTO} ${TRACE_PROTO} ${LOGS_PROTO} ${METRICS_PROTO}
    ${TRACE_SERVICE_PROTO} ${LOGS_SERVICE_PROTO} ${METRICS_SERVICE_PROTO})

include_directories("${GENERATED_PROTOBUF_PATH}")

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
  ${LOGS_SERVICE_GRPCPB_CPP_FILE}
  ${METRICS_SERVICE_PB_CPP_FILE}
  ${METRICS_SERVICE_GRPC_PB_CPP_FILE})

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
