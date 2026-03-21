# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(common_logs_foo_library foo_library.h foo_library.cc)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_compile_definitions(common_logs_foo_library
                             PRIVATE OPENTELEMETRY_BUILD_IMPORT_DLL)
endif()

target_include_directories(common_logs_foo_library
                           PUBLIC $<BUILD_INTERFACE:${EXAMPLES_COMMON_DIR}>)

target_link_libraries(common_logs_foo_library PUBLIC opentelemetry-cpp::api)

# Internal deprecated API calling should not failed the build in maintainer mode
if(OTELCPP_MAINTAINER_MODE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    get_target_property(COMMON_LOGS_FOO_COMPILE_OPTS common_logs_foo_library
                        COMPILE_OPTIONS)
    list(REMOVE_ITEM COMMON_LOGS_FOO_COMPILE_OPTS
         -Wno-error=deprecated-declarations)
    set_target_properties(
      common_logs_foo_library PROPERTIES COMPILE_OPTIONS
                                         "${COMMON_LOGS_FOO_COMPILE_OPTS}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    get_target_property(COMMON_LOGS_FOO_COMPILE_OPTS common_logs_foo_library
                        COMPILE_OPTIONS)
    list(REMOVE_ITEM COMMON_LOGS_FOO_COMPILE_OPTS /wd4996)
    list(APPEND COMMON_LOGS_FOO_COMPILE_OPTS /w34996)
    set_target_properties(
      common_logs_foo_library PROPERTIES COMPILE_OPTIONS
                                         "${COMMON_LOGS_FOO_COMPILE_OPTS}")
  endif()
endif()
