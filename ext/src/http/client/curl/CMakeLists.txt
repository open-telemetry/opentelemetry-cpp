find_package(CURL)
if(CURL_FOUND)
  add_library(opentelemetry_http_client_curl http_client_factory_curl.cc
                                             http_client_curl.cc)

  set_target_properties(opentelemetry_http_client_curl
                        PROPERTIES EXPORT_NAME http_client_curl)

  if(TARGET CURL::libcurl)
    target_link_libraries(opentelemetry_http_client_curl
                          PUBLIC opentelemetry_ext CURL::libcurl)
  else()
    target_include_directories(opentelemetry_http_client_curl
                               INTERFACE "${CURL_INCLUDE_DIRS}")
    target_link_libraries(opentelemetry_http_client_curl
                          PUBLIC opentelemetry_ext ${CURL_LIBRARIES})
  endif()

  install(
    TARGETS opentelemetry_http_client_curl
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
