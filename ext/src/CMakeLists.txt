if(WITH_ZPAGES)
  add_subdirectory(zpages)
endif()

add_subdirectory(http/client/curl)
if(BUILD_TESTING)
  add_subdirectory(http/client/nosend)
endif()
