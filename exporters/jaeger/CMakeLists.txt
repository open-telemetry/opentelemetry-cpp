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

find_package(thrift REQUIRED)

set(GENERATED_THRIFT_PATH
    "${CMAKE_BINARY_DIR}/generated/thrift-gen/cpp")

file(MAKE_DIRECTORY "${GENERATED_THRIFT_PATH}")

set(jaeger_thrift_gencpp_SOURCES
    ${GENERATED_THRIFT_PATH}/Agent.cpp
    ${GENERATED_THRIFT_PATH}/jaeger_types.cpp
    ${GENERATED_THRIFT_PATH}/Collector.cpp
)

add_custom_command(OUTPUT ${GENERATED_THRIFT_PATH}/Agent.cpp
    COMMAND ${THRIFT_COMPILER} --gen cpp -out ${GENERATED_THRIFT_PATH} -r ${CMAKE_CURRENT_SOURCE_DIR}/thrift/agent.thrift
)

add_custom_command(OUTPUT ${GENERATED_THRIFT_PATH}/Collector.cpp ${GENERATED_THRIFT_PATH}/jaeger_types.cpp
    COMMAND ${THRIFT_COMPILER} --gen cpp -out ${GENERATED_THRIFT_PATH} -r ${CMAKE_CURRENT_SOURCE_DIR}/thrift/jaeger.thrift
)

add_library(jae_trace_exporter ${jaeger_thrift_gencpp_SOURCES})

# if(BUILD_TESTING)
#   add_executable(jaeger_exporter_test )
#   target_link_libraries(jaeger_exporter_test ${GTEST_BOTH_LIBRARIES})
#                         ${CMAKE_THREAD_LIBS_INIT} jaeger_trace_exporter)
# 
#   gtest_add_tests()
# endif() # BUILD_TESTING
