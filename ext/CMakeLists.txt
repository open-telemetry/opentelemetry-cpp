add_subdirectory(src)
include_directories(include)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
