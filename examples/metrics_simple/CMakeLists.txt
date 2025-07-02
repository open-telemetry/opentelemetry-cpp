# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(metrics_ostream_example metrics_ostream.cc)

target_link_libraries(metrics_ostream_example
                      PRIVATE common_metrics_foo_library)

if(DEFINED OPENTELEMETRY_BUILD_DLL)
  target_compile_definitions(metrics_ostream_example
                             PRIVATE OPENTELEMETRY_BUILD_IMPORT_DLL)
  target_link_libraries(metrics_ostream_example
                        PRIVATE opentelemetry-cpp::opentelemetry_cpp)
else()
  target_link_libraries(
    metrics_ostream_example PRIVATE opentelemetry-cpp::metrics
                                    opentelemetry-cpp::ostream_metrics_exporter)
endif()

if(BUILD_TESTING)
  add_test(NAME examples.metrics_simple
           COMMAND "$<TARGET_FILE:metrics_ostream_example>")
  # Disable the metrics_simple example test due to sporadic segfaults on Windows
  # See https://github.com/open-telemetry/opentelemetry-cpp/issues/3458
  set_tests_properties(examples.metrics_simple PROPERTIES DISABLED TRUE)
endif()
