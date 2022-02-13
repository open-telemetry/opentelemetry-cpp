add_subdirectory(curl)
add_subdirectory(nosend)

add_library(http_client_factory http_client_factory.cc)
set_target_properties(http_client_factory PROPERTIES EXPORT_NAME
                                                     http_client_factory)
set(http_client_factory_libs http_client_curl)
if(${BUILD_TESTING})
  set(http_client_factory_libs ${http_client_factory_libs} http_client_nosend)
endif()

target_link_libraries(http_client_factory ${http_client_factory_libs})

install(
  TARGETS http_client_factory
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
