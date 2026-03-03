# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(NOT WIN32)
  add_executable(example_environment_carrier main.cc)

  if(DEFINED OPENTELEMETRY_BUILD_DLL)
    target_compile_definitions(example_environment_carrier
                               PRIVATE OPENTELEMETRY_BUILD_IMPORT_DLL)
    target_link_libraries(example_environment_carrier
                          PRIVATE opentelemetry-cpp::opentelemetry_cpp)
  else()
    target_link_libraries(
      example_environment_carrier
      PRIVATE opentelemetry-cpp::trace opentelemetry-cpp::ostream_span_exporter)
  endif()

  if(BUILD_TESTING)
    add_test(NAME examples.environment_carrier
             COMMAND "$<TARGET_FILE:example_environment_carrier>")
  endif()
endif()
