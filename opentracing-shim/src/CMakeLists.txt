set(this_target opentelemetry_opentracing_shim)
set(target_name opentracing_shim)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cc)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/../include/*.h)

add_library(${this_target} SHARED ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(${this_target} PROPERTIES EXPORT_NAME ${target_name})

target_include_directories(
  ${this_target}
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(${this_target} PUBLIC opentelemetry_api opentracing)

install(
  TARGETS ${this_target}
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()