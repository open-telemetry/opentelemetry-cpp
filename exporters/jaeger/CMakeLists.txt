include_directories(include)
include_directories(thrift-gen)

find_package(thrift REQUIRED)

set(JAEGER_THRIFT_GENCPP_SOURCES
    thrift-gen/Agent.cpp thrift-gen/jaeger_types.cpp thrift-gen/Collector.cpp
    thrift-gen/zipkincore_types.cpp)

set(JAEGER_EXPORTER_SOURCES
    src/jaeger_exporter.cc src/thrift_sender.cc src/udp_transport.cc
    src/recordable.cc src/TUDPTransport.cc)

add_library(jaeger_trace_exporter ${JAEGER_EXPORTER_SOURCES}
                                  ${JAEGER_THRIFT_GENCPP_SOURCES})
target_link_libraries(
  jaeger_trace_exporter
  PUBLIC opentelemetry_resources
  PRIVATE thrift::thrift)

if(MSVC)
  target_compile_definitions(jaeger_trace_exporter PUBLIC NOMINMAX)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(jaeger_trace_exporter
                               PUBLIC THRIFT_STATIC_DEFINE)
  endif()
endif()

if(BUILD_TESTING)
  add_executable(jaeger_recordable_test test/jaeger_recordable_test.cc)
  target_link_libraries(jaeger_recordable_test ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} jaeger_trace_exporter)

  gtest_add_tests(
    TARGET jaeger_recordable_test
    TEST_PREFIX exporter.
    TEST_LIST jaeger_recordable_test)
endif() # BUILD_TESTING
