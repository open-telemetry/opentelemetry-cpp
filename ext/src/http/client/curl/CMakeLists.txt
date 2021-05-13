find_package(CURL)
if(CURL_FOUND)
  add_library(http_client_curl http_client_factory_curl http_client_curl)

  set_target_properties(http_client_curl PROPERTIES EXPORT_NAME
                                                    http_client_curl)

  target_link_libraries(http_client_curl PUBLIC CURL::libcurl)

  install(
    TARGETS http_client_curl
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
