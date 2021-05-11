
if(NOT Protobuf_FOUND)
  find_package(Protobuf)
elseif(NOT gRPC_FOUND)
  find_package(gRPC)
if(NOT Protobuf_FOUND)
  message(WARNING "Aborting grpc example build, protobuf not found.")
elseif(NOT gRPC_FOUND)
  message(WARNING "Aborting grpc example build, grpc not found.")
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
  set(_GRPC_GRPCPP gRPC::grpc++)
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

  include_directories(
    ${CMAKE_SOURCE_DIR}/exporters/ostream/include
    ${CMAKE_SOURCE_DIR}/ext/include ${CMAKE_SOURCE_DIR}/api/include/
    ${CMAKE_SOURCE_DIR/})

  include_directories(${CMAKE_CURRENT_BINARY_DIR}/protos)

  get_filename_component(
    hw_proto ${CMAKE_CURRENT_SOURCE_DIR}/protos/messages.proto ABSOLUTE)
  get_filename_component(hw_proto_path "${hw_proto}" PATH)

  set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.cc")
  set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.pb.h")
  set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.cc")
  set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/messages.grpc.pb.h")

  add_custom_command(
    OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}"
           "${hw_grpc_hdrs}"
    COMMAND
      ${_PROTOBUF_PROTOC} ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
      --cpp_out "${CMAKE_CURRENT_BINARY_DIR}" -I "${hw_proto_path}"
      --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}" "${hw_proto}"
    DEPENDS "${hw_proto}")

  include_directories("${CMAKE_CURRENT_BINARY_DIR}")

  add_library(grpc_foo_library grpc_foo_lib/foo_split.cc)

  add_library(hw_grpc_proto ${hw_grpc_srcs} ${hw_grpc_hdrs} ${hw_proto_srcs}
                            ${hw_proto_hdrs})

  target_link_libraries(hw_grpc_proto ${_REFLECTION} ${_GRPC_GRPCPP}
                        ${_PROTOBUF_LIBPROTOBUF})

  foreach(_target client server)
    add_executable(${_target} "${_target}.cc")
    target_link_libraries(
      ${_target}
      hw_grpc_proto
      grpc_foo_library
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF}
      ${CMAKE_THREAD_LIBS_INIT}
      ${CORE_RUNTIME_LIBS}
      opentelemetry_trace
      opentelemetry_exporter_ostream_span)
  endforeach()
endif()
