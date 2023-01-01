#set(this_target opentelemetry_opentracing_shim)

#message("CMAKE_CURRENT_LIST_DIR " ${CMAKE_CURRENT_LIST_DIR})
#message("CMAKE_CURRENT_SOURCE_DIR " ${CMAKE_CURRENT_SOURCE_DIR})
#message("CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})
#message("PROJECT_SOURCE_DIR " ${PROJECT_SOURCE_DIR})

#add_library(${this_target} INTERFACE)
#target_include_directories(
#  ${this_target}
#  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
#         "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/opentracing-cpp/include>"
#         "$<INSTALL_INTERFACE:include>")
#
#set_target_properties(${this_target} PROPERTIES EXPORT_NAME "opentracing-shim")
#target_link_libraries(${this_target} INTERFACE ${this_target} opentracing-cpp)
#
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()
#
#install(
#  TARGETS ${this_target}
#  EXPORT "${PROJECT_NAME}-target"
#  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
#
#install(
#  DIRECTORY include/opentelemetry/opentracing-shim
#  DESTINATION include/opentelemetry/
#  FILES_MATCHING
#  PATTERN "*.h")

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif() # BUILD_TESTING