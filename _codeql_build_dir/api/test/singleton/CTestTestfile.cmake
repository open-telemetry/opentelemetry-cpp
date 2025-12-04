# CMake generated Testfile for 
# Source directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/api/test/singleton
# Build directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/api/test/singleton
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(singleton.SingletonTest.Uniqueness "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/api/test/singleton/singleton_test" "--gtest_filter=SingletonTest.Uniqueness")
set_tests_properties(singleton.SingletonTest.Uniqueness PROPERTIES  DEF_SOURCE_LINE "singleton_test.cc:363" SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;2;add_test;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;0;;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake;510;cmake_language;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/api/test/singleton/CMakeLists.txt;52;gtest_add_tests;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/api/test/singleton/CMakeLists.txt;0;")
