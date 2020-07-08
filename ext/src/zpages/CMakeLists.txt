add_library(opentelemetry_zpages tracez_processer.cc)

target_link_libraries(opentelemetry_zpages ${CMAKE_THREAD_LIBS_INIT}
	opentelemetry_trace)
