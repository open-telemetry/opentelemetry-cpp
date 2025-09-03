# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_library(
  opentelemetry_opentracing_shim src/shim_utils.cc src/span_shim.cc
                                 src/span_context_shim.cc src/tracer_shim.cc)

set_target_properties(opentelemetry_opentracing_shim
                      PROPERTIES EXPORT_NAME opentracing_shim)
set_target_version(opentelemetry_opentracing_shim)

target_include_directories(
  opentelemetry_opentracing_shim
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(opentelemetry_opentracing_shim PUBLIC opentelemetry_api)

# link to the shared library target (OpenTracing::opentracing) if it exists
# otherwise use the static library target.
if(TARGET OpenTracing::opentracing)
  target_link_libraries(opentelemetry_opentracing_shim
                        PUBLIC OpenTracing::opentracing)
else()
  target_link_libraries(opentelemetry_opentracing_shim
                        PUBLIC OpenTracing::opentracing-static)
endif()

otel_add_component(
  COMPONENT
  shims_opentracing
  TARGETS
  opentelemetry_opentracing_shim
  FILES_DIRECTORY
  "include/opentelemetry/opentracingshim"
  FILES_DESTINATION
  "include/opentelemetry/"
  FILES_MATCHING
  PATTERN
  "*.h")

if(BUILD_TESTING)
  foreach(testname propagation_test shim_utils_test span_shim_test
                   span_context_shim_test tracer_shim_test)
    add_executable(${testname} "test/${testname}.cc")
    target_link_libraries(
      ${testname} PRIVATE ${GTEST_BOTH_LIBRARIES} Threads::Threads
                          opentelemetry_opentracing_shim)
    gtest_add_tests(
      TARGET ${testname}
      TEST_PREFIX opentracing_shim.
      TEST_LIST ${testname})
  endforeach()
endif() # BUILD_TESTING
