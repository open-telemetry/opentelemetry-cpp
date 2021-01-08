include_directories(include)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
