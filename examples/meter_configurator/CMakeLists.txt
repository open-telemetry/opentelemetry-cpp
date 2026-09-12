# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_meter_configurator main.cc)
target_link_libraries(
  example_meter_configurator
  PRIVATE opentelemetry-cpp::metrics
          opentelemetry-cpp::ostream_metrics_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.meter_configurator
           COMMAND "$<TARGET_FILE:example_meter_configurator>")
endif()
