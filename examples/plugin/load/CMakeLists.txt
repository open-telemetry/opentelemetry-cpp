add_executable(load_plugin_example main.cc)
target_link_libraries(load_plugin_example opentelemetry_api ${CMAKE_DL_LIBS})
