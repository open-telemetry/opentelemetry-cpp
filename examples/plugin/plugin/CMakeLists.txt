add_library(example_plugin SHARED tracer.cc factory_impl.cc)
target_link_libraries(example_plugin opentelemetry_api)
