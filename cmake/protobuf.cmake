# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED gRPC_PROVIDER AND NOT gRPC_PROVIDER STREQUAL "package" AND TARGET libprotobuf)
  # gRPC was fetched and built protobuf as a submodule
  set(Protobuf_PROVIDER "grpc_submodule")
  otel_add_thirdparty_package(
    PACKAGE_NAME "Protobuf"
    VERSION_REGEX "\"cpp\"[ \t]*:[ \t]*\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\""
    VERSION_FILE "\${grpc_SOURCE_DIR}/third_party/protobuf/version.json"
  )
else()
  # Protobuf 3.22+ depends on abseil-cpp and must be found using the cmake
  # find_package CONFIG search mode. The following attempts to find Protobuf
  # using the CONFIG mode first, and if not found, falls back to the MODULE
  # mode. See https://gitlab.kitware.com/cmake/cmake/-/issues/24321 for more
  # details.

  otel_add_thirdparty_package(
    PACKAGE_NAME "Protobuf"
    PACKAGE_SEARCH_MODES "CONFIG" "MODULE"
    FETCH_NAME "protobuf"
    FETCH_GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
    FETCH_GIT_TAG "${protobuf_GIT_TAG}"
    FETCH_CMAKE_ARGS
      protobuf_INSTALL=${OPENTELEMETRY_INSTALL}
      protobuf_BUILD_TESTS=OFF
      protobuf_BUILD_EXAMPLES=OFF
      protobuf_WITH_ZLIB=OFF
    VERSION_REGEX "\"cpp\"[ \t]*:[ \t]*\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\""
    VERSION_FILE "\${protobuf_SOURCE_DIR}/version.json"
  )
endif()

if(TARGET libprotobuf)
  set_target_properties(libprotobuf PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_CLANG_TIDY "" CXX_INCLUDE_WHAT_YOU_USE "")
endif()

if(NOT TARGET protobuf::libprotobuf)
  message(FATAL_ERROR "A required protobuf target (protobuf::libprotobuf) was not found")
endif()

if(CMAKE_CROSSCOMPILING)
  find_program(PROTOBUF_PROTOC_EXECUTABLE protoc)
else()
  if(NOT TARGET protobuf::protoc)
    message(FATAL_ERROR "A required protobuf target (protobuf::protoc) was not found")
  endif()
  set(PROTOBUF_PROTOC_EXECUTABLE "$<TARGET_FILE:protobuf::protoc>")
endif()

message(STATUS "PROTOBUF_PROTOC_EXECUTABLE=${PROTOBUF_PROTOC_EXECUTABLE}")
