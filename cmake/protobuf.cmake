# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

get_property(_gRPC_PROVIDER DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_gRPC_PROVIDER)

if(DEFINED _gRPC_PROVIDER AND NOT _gRPC_PROVIDER STREQUAL "package" AND TARGET libprotobuf)
  # gRPC was fetched and built protobuf as a submodule
  otel_add_thirdparty_package(
    PACKAGE_NAME "Protobuf"
    PACKAGE_NAMESPACE "protobuf"
    REQUIRED_TARGETS "protobuf::libprotobuf"
    VERSION_REGEX "\"cpp\"[ \t]*:[ \t]*\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\""
    VERSION_FILE "\${grpc_SOURCE_DIR}/third_party/protobuf/version.json"
  )
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_Protobuf_PROVIDER "grpc_submodule")
else()
  otel_add_thirdparty_package(
    PACKAGE_NAME "Protobuf"
    PACKAGE_SEARCH_MODES "CONFIG" "MODULE"
    PACKAGE_NAMESPACE "protobuf"
    FETCH_NAME "protobuf"
    FETCH_GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
    FETCH_GIT_TAG "${protobuf_GIT_TAG}"
    FETCH_CMAKE_ARGS
      protobuf_INSTALL=${OPENTELEMETRY_INSTALL}
      protobuf_BUILD_TESTS=OFF
      protobuf_BUILD_EXAMPLES=OFF
    REQUIRED_TARGETS "protobuf::libprotobuf"
    VERSION_REGEX "\"cpp\"[ \t]*:[ \t]*\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\""
    VERSION_FILE "\${protobuf_SOURCE_DIR}/version.json"
  )
endif()

# Protobuf 3.22+ depends on abseil-cpp and must be found using the cmake
# find_package CONFIG search mode. The following attempts to find Protobuf
# using the CONFIG mode first, and if not found, falls back to the MODULE
# mode. See https://gitlab.kitware.com/cmake/cmake/-/issues/24321 for more
# details.
get_property(_Protobuf_VERSION DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_Protobuf_VERSION)
if(DEFINED _Protobuf_VERSION AND _Protobuf_VERSION VERSION_LESS 3.22.0)
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_Protobuf_SEARCH_MODE "")
else()
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_Protobuf_SEARCH_MODE "CONFIG")
endif()

if(TARGET libprotobuf)
  set_target_properties(libprotobuf PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_CLANG_TIDY "" CXX_INCLUDE_WHAT_YOU_USE "")
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
