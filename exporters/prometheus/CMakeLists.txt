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
if(NOT TARGET prometheus-cpp::core)
  find_package(prometheus-cpp CONFIG REQUIRED)
endif()
if(WITH_METRICS_PREVIEW)
  add_library(
    prometheus_exporter_deprecated
    src/prometheus_exporter.cc src/prometheus_collector.cc
    src/prometheus_exporter_utils.cc)
  target_include_directories(
    prometheus_exporter_deprecated
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  set(PROMETHEUS_EXPORTER_TARGETS_DEPRECATED prometheus_exporter_deprecated)
  if(TARGET pull)
    list(APPEND PROMETHEUS_EXPORTER_TARGETS_DEPRECATED pull)
  endif()
  if(TARGET core)
    list(APPEND PROMETHEUS_EXPORTER_TARGETS_DEPRECATED core)
  endif()
  target_link_libraries(
    prometheus_exporter_deprecated
    PUBLIC opentelemetry_metrics_deprecated prometheus-cpp::pull
           prometheus-cpp::core)
  install(
    TARGETS ${PROMETHEUS_EXPORTER_TARGETS_DEPRECATED}
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
else()

  add_library(prometheus_exporter src/exporter.cc src/collector.cc
                                  src/exporter_utils.cc)
  target_include_directories(
    prometheus_exporter
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
           "$<INSTALL_INTERFACE:include>")

  set(PROMETHEUS_EXPORTER_TARGETS prometheus_exporter)
  if(TARGET pull)
    list(APPEND PROMETHEUS_EXPORTER_TARGETS pull)
  endif()
  if(TARGET core)
    list(APPEND PROMETHEUS_EXPORTER_TARGETS core)
  endif()
  target_link_libraries(
    prometheus_exporter PUBLIC opentelemetry_metrics prometheus-cpp::pull
                               prometheus-cpp::core)
  install(
    TARGETS ${PROMETHEUS_EXPORTER_TARGETS}
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
endif()
