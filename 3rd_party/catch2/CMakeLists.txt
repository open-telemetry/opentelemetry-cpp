include_directories(include)

add_library(catch2_with_main src/catch2_main.cc)
target_include_directories(catch2_with_main PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
