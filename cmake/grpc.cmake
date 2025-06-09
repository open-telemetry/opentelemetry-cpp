# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Including the CMakeFindDependencyMacro resolves an error from
# gRPCConfig.cmake on some grpc versions. See
# https://github.com/grpc/grpc/pull/33361 for more details.
include(CMakeFindDependencyMacro)

otel_add_thirdparty_package(
  PACKAGE_NAME "gRPC"
  PACKAGE_SEARCH_MODES "CONFIG"
  FETCH_NAME "grpc"
  FETCH_GIT_REPOSITORY "https://github.com/grpc/grpc.git"
  FETCH_GIT_TAG "${grpc_GIT_TAG}"
  FETCH_GIT_SUBMODULES
          "third_party/re2"
          "third_party/abseil-cpp"
          "third_party/protobuf"
          "third_party/cares/cares"
          "third_party/boringssl-with-bazel"
  FETCH_CMAKE_ARGS
    "-DgRPC_INSTALL=${OPENTELEMETRY_INSTALL}"
    "-DgRPC_BUILD_TESTS=OFF"
    "-DgRPC_BUILD_GRPC_CPP_PLUGIN=ON"
    "-DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF"
    "-DgRPC_BUILD_GRPCPP_OTEL_PLUGIN=OFF"
    "-DRE2_BUILD_TESTING=OFF"
    "-DgRPC_ZLIB_PROVIDER=package"
    "-DgRPC_RE2_PROVIDER=module"
    "-DgRPC_PROTOBUF_PROVIDER=module"
    "-DgRPC_ABSL_PROVIDER=module"
    "-DgRPC_CARES_PROVIDER=module"
  VERSION_REGEX "#define[ \t]+GRPC_CPP_VERSION_STRING[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?)\""
  VERSION_FILE "\${grpc_SOURCE_DIR}/include/grpcpp/version_info.h"
)

if(TARGET grpc++)
  set_target_properties(grpc++ PROPERTIES POSITION_INDEPENDENT_CODE ON
                        CXX_INCLUDE_WHAT_YOU_USE "" CXX_CLANG_TIDY "")
endif()

if(TARGET grpc++ AND NOT TARGET gRPC::grpc++)
  add_library(gRPC::grpc++ ALIAS grpc++)
endif()

if(TARGET grpc_cpp_plugin AND NOT TARGET gRPC::grpc_cpp_plugin)
  add_executable(gRPC::grpc_cpp_plugin ALIAS grpc_cpp_plugin)
endif()

if(NOT TARGET gRPC::grpc++)
  message(FATAL_ERROR "A required gRPC target (grpc++) was not found")
endif()

if(CMAKE_CROSSCOMPILING)
  find_program(gRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  if(NOT TARGET gRPC::grpc_cpp_plugin)
    message(FATAL_ERROR "A required gRPC target (grpc_cpp_plugin) was not found")
  endif()
  set(gRPC_CPP_PLUGIN_EXECUTABLE "$<TARGET_FILE:gRPC::grpc_cpp_plugin>")
endif()

message(STATUS "gRPC_CPP_PLUGIN_EXECUTABLE=${gRPC_CPP_PLUGIN_EXECUTABLE}")
