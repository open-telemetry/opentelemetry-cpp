find_package(CURL)
if(CURL_FOUND)
  add_library(opentelemetry_curl_factory http_client_factory_curl)
endif()
