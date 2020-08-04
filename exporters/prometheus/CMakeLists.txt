include_directories(include)

find_package(prometheus-cpp CONFIG REQUIRED)
add_library(prometheus_exporter src/prometheus_exporter.cc src/prometheus_collector.cc src/prometheus_exporter_utils.cc)

add_executable(prometheus_exporter_test test/prometheus_exporter_test.cc test/prometheus_collector_test.cc test/prometheus_exporter_utils_test.cc)

target_link_libraries(
        prometheus_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
        prometheus_exporter prometheus-cpp::pull)
gtest_add_tests(TARGET prometheus_exporter_test TEST_PREFIX exporter. TEST_LIST
        prometheus_exporter_test)