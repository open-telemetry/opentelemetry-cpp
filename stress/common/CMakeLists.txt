add_library(stress STATIC stress.cc)

# Include directory for the throughput library
target_include_directories(stress PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set C++ standard
set_target_properties(
  stress
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO)
