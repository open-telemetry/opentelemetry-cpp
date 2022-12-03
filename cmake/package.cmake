
if (UNIX AND NOT APPLE)
set(CMAKE_PROJECT_NAME "opentelemetry-cpp")

include(cmake/ParseOsRelease.cmake)
execute_process(
    COMMAND lsb_release -si
    OUTPUT_VARIABLE distribution
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (distribution STREQUAL "")
    set(distribution ${OS_RELEASE_NAME})
endif()

set(CPACK_SYSTEM_NAME "${OS_RELEASE_ID}-${OS_RELEASE_VERSION_ID}")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
set(CPACK_PACKAGE_DESCRIPTION "OpenTelemetry C++ for Linux")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenTelemetry C++ for Linux - C++ Implementation of OpenTelemetry Specification")
set(CPACK_PACKAGE_VENDOR "OpenTelemetry")
set(CPACK_PACKAGE_CONTACT "OpenTelemetry-cpp")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://opentelemetry.io/")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${OPENTELEMETRY_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

message("file name ${CPACK_PACKAGE_FILE_NAME}")
if(distribution STREQUAL "Debian" OR distribution STREQUAL "Ubuntu")
    set(CPACK_GENERATOR "DEB")
    set(INSTALL_LIB_DIR
        ${CMAKE_INSTALL_PREFIX}/lib/${CPACK_DEBIAN_ARCHITECTURE}-linux-gnu)
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    #set(CPACK_COMPONENTS_ALL headers libraries)
    set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    message("-- Package name: ${CPACK_PACKAGE_FILE_NAME}.deb")
elseif(
    distribution MATCHES "RedHat.*"
    OR distribution MATCHES "openSUSE.*"
    OR distribution STREQUAL "Fedora")
    set(CPACK_GENERATOR "RPM")
    set(INSTALL_LIB_DIR
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu)
    set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
endif()
endif()
include(CPack)