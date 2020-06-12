#!/bin/bash
pushd $1
echo Running tests in `pwd`

echo Running API tests...
./api/test/core/timestamp_test
./api/test/nostd/function_ref_test
./api/test/nostd/shared_ptr_test
./api/test/nostd/span_test
./api/test/nostd/string_view_test
./api/test/nostd/unique_ptr_test
./api/test/nostd/utility_test
./api/test/plugin/dynamic_load_test
./api/test/trace/key_value_iterable_view_test
./api/test/trace/noop_test
./api/test/trace/provider_test
./api/test/trace/span_id_benchmark
./api/test/trace/span_id_test
./api/test/trace/trace_flags_test
./api/test/trace/trace_id_test

echo Running SDK tests...
./sdk/test/common/atomic_unique_ptr_test
./sdk/test/common/circular_buffer_benchmark
./sdk/test/common/circular_buffer_range_test
./sdk/test/common/circular_buffer_test
./sdk/test/common/fast_random_number_generator_test
./sdk/test/common/random_benchmark
./sdk/test/common/random_fork_test
./sdk/test/common/random_test
./sdk/test/trace/simple_processor_test
./sdk/test/trace/span_data_test
./sdk/test/trace/tracer_provider_test
./sdk/test/trace/tracer_test

echo Running Examples...
./examples/TraceStreamer/TraceStreamer
./examples/plugin/load/load_plugin_example

popd
