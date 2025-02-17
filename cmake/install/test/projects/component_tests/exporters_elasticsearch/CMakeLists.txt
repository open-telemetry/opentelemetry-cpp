cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-exporters-elasticsearch-install-test LANGUAGES CXX)
message(STATUS "Testing the exporters_elasticsearch_test component install")
find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_elasticsearch)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(exporters_elasticsearch_test
               ${INSTALL_TEST_SRC_DIR}/test_exporters_elasticsearch.cc)
target_link_libraries(
  exporters_elasticsearch_test
  PRIVATE opentelemetry-cpp::elasticsearch_log_record_exporter GTest::GTest
          GTest::Main)

gtest_discover_tests(exporters_elasticsearch_test)
