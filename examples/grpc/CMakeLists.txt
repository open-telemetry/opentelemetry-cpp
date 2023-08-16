# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Proto file
get_filename_component(proto_file "./protos/messages.proto" ABSOLUTE)
get_filename_component(proto_file_path "${proto_file}" PATH)

message("PATH:${proto_file_path}:${proto_file}")
# Generated sources
set(example_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.cc")
set(example_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.h")
set(example_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.cc")
set(example_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.h")

add_custom_command(
  OUTPUT "${example_proto_srcs}" "${example_proto_hdrs}" "${example_grpc_srcs}"
         "${example_grpc_hdrs}"
  COMMAND
    ${PROTOBUF_PROTOC_EXECUTABLE} ARGS "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
    "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}" "--proto_path=${proto_file_path}"
    "--plugin=protoc-gen-grpc=${gRPC_CPP_PLUGIN_EXECUTABLE}" "${proto_file}")

add_library(example_grpc_proto ${example_grpc_srcs} ${example_grpc_hdrs}
                               ${example_proto_srcs} ${example_proto_hdrs})

patch_protobuf_targets(example_grpc_proto)

include_directories(
  ${CMAKE_SOURCE_DIR}/exporters/ostream/include ${CMAKE_SOURCE_DIR}/ext/include
  ${CMAKE_SOURCE_DIR}/api/include/ ${CMAKE_SOURCE_DIR/})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(TARGET protobuf::libprotobuf)
  target_link_libraries(example_grpc_proto PUBLIC gRPC::grpc++
                                                  protobuf::libprotobuf)
else()
  target_include_directories(example_grpc_proto PUBLIC ${Protobuf_INCLUDE_DIRS})
  target_link_libraries(example_grpc_proto PUBLIC gRPC::grpc++
                                                  ${Protobuf_LIBRARIES})
endif()
if(WITH_ABSEIL)
  target_link_libraries(example_grpc_proto PUBLIC absl::bad_variant_access)
endif()

foreach(_target client server)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target} example_grpc_proto opentelemetry_trace
                        opentelemetry_exporter_ostream_span)
  patch_protobuf_targets(${_target})
endforeach()
