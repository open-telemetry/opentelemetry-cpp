# Copyright 2020, OpenTelemetry Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_directories(include)

find_package(prometheus-cpp CONFIG REQUIRED)
add_library(
  prometheus_exporter src/prometheus_exporter.cc src/prometheus_collector.cc
                      src/prometheus_exporter_utils.cc)

add_executable(
  prometheus_exporter_test
  test/prometheus_exporter_test.cc test/prometheus_collector_test.cc
  test/prometheus_exporter_utils_test.cc)

target_link_libraries(
  prometheus_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
  prometheus_exporter prometheus-cpp::pull)
gtest_add_tests(TARGET prometheus_exporter_test TEST_PREFIX exporter. TEST_LIST
                prometheus_exporter_test)
