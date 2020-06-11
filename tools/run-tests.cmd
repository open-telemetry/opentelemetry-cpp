@echo off
pushd %1
echo Running tests in %CD%

echo Running API tests...
api\test\core\Release\timestamp_test.exe
api\test\nostd\Release\function_ref_test.exe
api\test\nostd\Release\shared_ptr_test.exe
api\test\nostd\Release\span_test.exe
api\test\nostd\Release\string_view_test.exe
api\test\nostd\Release\unique_ptr_test.exe
api\test\nostd\Release\utility_test.exe
api\test\plugin\Release\dynamic_load_test.exe
api\test\trace\Release\key_value_iterable_view_test.exe
api\test\trace\Release\noop_test.exe
api\test\trace\Release\provider_test.exe
api\test\trace\Release\span_id_benchmark.exe
api\test\trace\Release\span_id_test.exe
api\test\trace\Release\trace_flags_test.exe
api\test\trace\Release\trace_id_test.exe

echo Running SDK tests...
sdk\test\common\Release\atomic_unique_ptr_test.exe
sdk\test\common\Release\circular_buffer_benchmark.exe
sdk\test\common\Release\circular_buffer_range_test.exe
sdk\test\common\Release\circular_buffer_test.exe
sdk\test\common\Release\fast_random_number_generator_test.exe
sdk\test\common\Release\random_benchmark.exe
sdk\test\common\Release\random_fork_test.exe
sdk\test\common\Release\random_test.exe
sdk\test\trace\Release\simple_processor_test.exe
sdk\test\trace\Release\span_data_test.exe
sdk\test\trace\Release\tracer_provider_test.exe
sdk\test\trace\Release\tracer_test.exe

echo Running Examples...
examples\TraceStreamer\Release\TraceStreamer.exe
examples\plugin\load\Release\load_plugin_example.exe

popd
