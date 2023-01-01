set(THIS_TARGET opentelemetry_opentracing_shim)
set(TARGET_NAME opentracing_shim)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include
  ${CMAKE_SOURCE_DIR}/third_party/opentracing-cpp/include
  ${CMAKE_SOURCE_DIR}/third_party/opentracing-cpp/build/include
  ${CMAKE_SOURCE_DIR}/third_party/opentracing-cpp/3rd_party/include)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cc)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/../include/*.h)

add_library(${THIS_TARGET} SHARED ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(${THIS_TARGET} PROPERTIES EXPORT_NAME ${TARGET_NAME})

target_include_directories(
  ${THIS_TARGET}
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")

target_link_libraries(${THIS_TARGET} PUBLIC opentelemetry_api)

install(
  TARGETS ${THIS_TARGET}
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()