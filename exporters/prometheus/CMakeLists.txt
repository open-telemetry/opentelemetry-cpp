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

find_package(prometheus-cpp CONFIG REQUIRED)

add_library(prometheus_exporter src/prometheus_collector.cc
                                src/prometheus_exporter_utils.cc)

target_include_directories(
  prometheus_exporter
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(prometheus_exporter PUBLIC opentelemetry_metrics
                                                 prometheus-cpp::core)

install(
  TARGETS prometheus_exporter
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/exporters/prometheus
  DESTINATION include/opentelemetry/exporters/
  FILES_MATCHING
  PATTERN "*.h")

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
