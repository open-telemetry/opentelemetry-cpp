# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_tracer_configurator main.cc)
target_link_libraries(
  example_tracer_configurator PRIVATE opentelemetry-cpp::trace
                                      opentelemetry-cpp::ostream_span_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.tracer_configurator
           COMMAND "$<TARGET_FILE:example_tracer_configurator>")
endif()
