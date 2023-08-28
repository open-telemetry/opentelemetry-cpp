include(cmake/CPM.cmake)
set(CPM_USE_LOCAL_PACKAGES ON)


function(add_external_dependencies)


#
# Do we need HTTP CLIENT CURL ?
#

if(WITH_OTLP_HTTP
   OR WITH_ELASTICSEARCH
   OR WITH_ZIPKIN
   OR BUILD_W3CTRACECONTEXT_TEST
   OR WITH_EXAMPLES_HTTP)
  set(WITH_HTTP_CLIENT_CURL ON)
else()
  set(WITH_HTTP_CLIENT_CURL OFF)
endif()

#
# Do we need CURL ?
#

if((NOT WITH_API_ONLY) AND WITH_HTTP_CLIENT_CURL)
  # No specific version required.
  CPMAddPackage("gh:curl/curl#curl-8_2_1")
  message(STATUS "Found CURL: ${CURL_LIBRARIES}, version ${CURL_VERSION}")
endif()


#
# Do we need NLOHMANN_JSON ?
#

if(WITH_ELASTICSEARCH
   OR WITH_ZIPKIN
   OR WITH_OTLP_HTTP
   OR WITH_ZPAGES
   OR BUILD_W3CTRACECONTEXT_TEST
   OR WITH_ETW)
  set(USE_NLOHMANN_JSON ON)
else()
  set(USE_NLOHMANN_JSON OFF)
endif()

if((NOT WITH_API_ONLY) AND USE_NLOHMANN_JSON)
    CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.11.2
        GITHUB_REPOSITORY nlohmann/json
        GITHUB_TAG v3.11.2
        OPTIONS
        "JSON_BuildTests OFF"
    )
endif()


if(WITH_ABSEIL)
  find_package(absl CONFIG REQUIRED)
endif()

if(WITH_PROMETHEUS)
    CPMAddPackage(
        NAME prometheus-cpp
        GITHUB_REPOSITORY jupp0r/prometheus-cpp
        GIT_TAG master
        OPTIONS
        "ENABLE_TESTING OFF"
    )
endif()


if(BUILD_TESTING)
    CPMAddPackage(
        NAME googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG "main"
        OPTIONS 
            "INSTALL_GTEST OFF"
    )
    
    CPMAddPackage("gh:google/benchmark#main")
endif()

if(WITH_OPENTRACING)
    CPMAddPackage(
        NAME opentracing-cpp
        GITHUB_REPOSITORY opentracing/opentracing-cpp
        GIT_TAG master
        OPTIONS
        "BUILD_TESTING OFF"
    )
endif()

endfunction()
