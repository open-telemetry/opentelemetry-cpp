# CMake generated Testfile for 
# Source directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/examples/plugin
# Build directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/examples/plugin
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples.plugin "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/examples/plugin/load/load_plugin_example" "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/examples/plugin/plugin/libexample_plugin.so" "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/examples/plugin/load/empty_config.txt")
set_tests_properties(examples.plugin PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/examples/plugin/CMakeLists.txt;12;add_test;/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/examples/plugin/CMakeLists.txt;0;")
subdirs("load")
subdirs("plugin")
