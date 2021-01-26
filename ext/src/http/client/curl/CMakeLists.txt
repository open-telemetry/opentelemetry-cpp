find_package(CURL)
if(CURL_FOUND)
  add_library(http_client_curl http_client_factory_curl http_client_curl)
endif()
