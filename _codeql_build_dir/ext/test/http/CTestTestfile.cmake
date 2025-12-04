# CMake generated Testfile for 
# Source directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/test/http
# Build directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/ext/test/http
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ext.http.urlparser.*/UrlParserTests.BasicTests/* "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/ext/test/http/url_parser_test" "--gtest_filter=*/UrlParserTests.BasicTests/*")
set_tests_properties(ext.http.urlparser.*/UrlParserTests.BasicTests/* PROPERTIES  DEF_SOURCE_LINE "url_parser_test.cc:101" SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;2;add_test;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;0;;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake;510;cmake_language;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/test/http/CMakeLists.txt;21;gtest_add_tests;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/test/http/CMakeLists.txt;0;")
add_test(ext.http.urlparser.*/UrlDecoderTests.BasicTests/* "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/ext/test/http/url_parser_test" "--gtest_filter=*/UrlDecoderTests.BasicTests/*")
set_tests_properties(ext.http.urlparser.*/UrlDecoderTests.BasicTests/* PROPERTIES  DEF_SOURCE_LINE "url_parser_test.cc:132" SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;2;add_test;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake:510:EVAL;0;;/usr/local/share/cmake-3.31/Modules/GoogleTest.cmake;510;cmake_language;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/test/http/CMakeLists.txt;21;gtest_add_tests;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/test/http/CMakeLists.txt;0;")
