add_subdirectory(curl)
add_subdirectory(nosend)

find_package(CURL)
if(CURL_FOUND)
  add_library(http_client_factory http_client_factory.cc)
  set_target_properties(http_client_factory PROPERTIES EXPORT_NAME
                                                       http_client_factory)
  set(http_client_factory_libs http_client_curl)
  if(TARGET CURL::libcurl)
    set(http_client_factory_libs ${http_client_factory_libs} CURL::libcurl)
  else()
    set(http_client_factory_libs ${http_client_factory_libs} ${CURL_LIBRARIES})
    target_include_directories(http_client_factory_libs
                               INTERFACE "${CURL_INCLUDE_DIRS}")
  endif()
  if(${BUILD_TESTING})
    set(http_client_factory_libs ${http_client_factory_libs} http_client_nosend)
  endif()

  target_link_libraries(http_client_factory PUBLIC ${http_client_factory_libs})

  install(
    TARGETS http_client_factory
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
