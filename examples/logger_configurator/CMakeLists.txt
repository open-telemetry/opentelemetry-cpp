# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_executable(example_logger_configurator main.cc)
target_link_libraries(
  example_logger_configurator
  PRIVATE opentelemetry-cpp::logs
          opentelemetry-cpp::ostream_log_record_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.logger_configurator
           COMMAND "$<TARGET_FILE:example_logger_configurator>")
endif()
