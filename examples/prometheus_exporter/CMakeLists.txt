include_directories(${CMAKE_SOURCE_DIR}/exporters/prometheus/include)
find_package(prometheus-cpp CONFIG REQUIRED)

add_executable(prom_exp_example main.cc)
target_link_libraries(prom_exp_example ${CMAKE_THREAD_LIBS_INIT}
        prometheus_exporter prometheus-cpp::pull)
