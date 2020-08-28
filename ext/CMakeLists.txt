add_subdirectory(src)

find_package(nlohmann_json REQUIRED)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
