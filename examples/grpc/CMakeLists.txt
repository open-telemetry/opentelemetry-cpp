# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Proto file
get_filename_component(proto_file "./protos/messages.proto" ABSOLUTE)
get_filename_component(proto_file_path "${proto_file}" PATH)

message(STATUS "PATH:${proto_file_path}:${proto_file}")
# Generated sources
set(example_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.cc")
set(example_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.h")
set(example_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.cc")
set(example_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.h")

if(NOT TARGET gRPC::grpc_cpp_plugin)
  message(
    FATAL_ERROR
      "gRPC::grpc_cpp_plugin target not found. Please ensure that gRPC is installed and found with find_package."
  )
endif()

if(NOT DEFINED PROTOBUF_PROTOC_EXECUTABLE)
  if(NOT TARGET protobuf::protoc)
    message(
      FATAL_ERROR
        "protobuf::protoc target not found. Please ensure that Protobuf is installed and found with find_package."
    )
  endif()
  set(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc)
endif()

add_custom_command(
  OUTPUT "${example_proto_srcs}" "${example_proto_hdrs}" "${example_grpc_srcs}"
         "${example_grpc_hdrs}"
  COMMAND
    ${PROTOBUF_PROTOC_EXECUTABLE} ARGS "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
    "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}" "--proto_path=${proto_file_path}"
    "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    "${proto_file}")

add_library(example_grpc_proto ${example_grpc_srcs} ${example_grpc_hdrs}
                               ${example_proto_srcs} ${example_proto_hdrs})

# Disable include-what-you-use and clang-tidy on generated code.
set_target_properties(example_grpc_proto PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ""
                                                    CXX_CLANG_TIDY "")

target_include_directories(
  example_grpc_proto PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

if(TARGET protobuf::libprotobuf)
  target_link_libraries(example_grpc_proto PUBLIC gRPC::grpc++
                                                  protobuf::libprotobuf)
else()
  target_include_directories(example_grpc_proto PUBLIC ${Protobuf_INCLUDE_DIRS})
  target_link_libraries(example_grpc_proto PUBLIC gRPC::grpc++
                                                  ${Protobuf_LIBRARIES})
endif()

foreach(_target client server)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(
    ${_target} PRIVATE example_grpc_proto
                       opentelemetry-cpp::ostream_span_exporter)
endforeach()
