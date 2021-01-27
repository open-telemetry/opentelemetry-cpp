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
find_package(CURL)
if(CURL_FOUND)
  add_compile_definitions(WITH_CURL)
endif()
add_library(zipkin_trace_exporter src/zipkin_exporter.cc src/recordable.cc)
if(BUILD_TESTING)
  add_executable(zipkin_recordable_test test/zipkin_recordable_test.cc)

  target_link_libraries(zipkin_recordable_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} zipkin_trace_exporter)

  gtest_add_tests(
    TARGET zipkin_recordable_test
    TEST_PREFIX exporter.
    TEST_LIST zipkin_recordable_test)
endif() # BUILD_TESTING
