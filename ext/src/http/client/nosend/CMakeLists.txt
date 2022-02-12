add_library(
  http_client_nosend
  # "${CMAKE_SOURCE_DIR}/ext/include/opentelemetry/ext/http/client/nosend/http_client_nosend.h"
  http_client_nosend.cc)

set_target_properties(http_client_nosend PROPERTIES EXPORT_NAME
                                                    http_client_nosend)

if(MSVC)
  # Explicitly specify that we consume GTest from shared library. The rest of
  # code logic below determines whether we link Release or Debug flavor of the
  # library. These flavors have different prefix on Windows, gmock and gmockd
  # respectively.
  add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
  if(GMOCK_LIB)
    # unset GMOCK_LIB to force find_library to redo the lookup, as the cached
    # entry could cause linking to incorrect flavor of gmock and leading to
    # runtime error.
    unset(GMOCK_LIB CACHE)
  endif()
endif()
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  find_library(GMOCK_LIB gmockd PATH_SUFFIXES lib)
else()
  find_library(GMOCK_LIB gmock PATH_SUFFIXES lib)
endif()

target_link_libraries(http_client_nosend ${GMOCK_LIB} opentelemetry_api
                      opentelemetry_ext)
