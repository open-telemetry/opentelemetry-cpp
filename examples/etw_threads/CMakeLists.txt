# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

project(etw_threadpool)

add_executable(etw_threadpool main.cc)

target_link_libraries(
  etw_threadpool PRIVATE Threads::Threads opentelemetry-cpp::api
                         opentelemetry-cpp::etw_exporter)

if(BUILD_TESTING)
  add_test(NAME examples.etw_threads COMMAND "$<TARGET_FILE:etw_threadpool>")
endif()
