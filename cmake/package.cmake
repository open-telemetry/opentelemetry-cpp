
set(CPACK_PACKAGE_DESCRIPTION "OpenTelemetry C++ for Linux")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenTelemetry C++ for Linux - C++ Implementation of OpenTelemetry Specification")
set(CPACK_PACKAGE_VENDOR "OpenTelemetry")
set(CPACK_PACKAGE_CONTACT "OpenTelemetry-cpp")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://opentelemetry.io/")
set(CMAKE_PROJECT_NAME "opentelemetry-cpp")

option(TARBALL "Build a tarball package" OFF)

if (UNIX AND NOT APPLE)
    include(cmake/ParseOsRelease.cmake)
    set(CPACK_SYSTEM_NAME "${OS_RELEASE_ID}-${OS_RELEASE_VERSION_ID}")
    #set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${OPENTELEMETRY_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

    # Check if system is deb or rpm capable
    find_program(RPMCAPABLE rpmbuild)
    find_program(DEBCAPABLE dpkg-buildpackage)
    if (TARBALL)
        set(CPACK_GENERATOR "TGZ")
        message("-- Package name: ${CPACK_PACKAGE_FILE_NAME}.tar.gz")
    elseif (DEBCAPABLE MATCHES "NOTFOUND" AND RPMCAPABLE MATCHES "NOTFOUND")
        message(FATAL_ERROR "Required Package generator not found for either deb or rpm."
        " Install required package generation software and try again")
    elseif (NOT DEBCAPABLE MATCHES "NOTFOUND")
        if (NOT RPMCAPABLE MATCHES "NOTFOUND")
            message(WARNING "Both deb and rpm package generator found."
            "Selecting deb as default packager.")
        endif()
        set(CPACK_GENERATOR "DEB")
        set(INSTALL_LIB_DIR
            ${CMAKE_INSTALL_PREFIX}/lib/${CPACK_DEBIAN_ARCHITECTURE}-linux-gnu)
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
        #set(CPACK_COMPONENTS_ALL headers libraries)
        set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
        message("-- Package name: ${CPACK_PACKAGE_FILE_NAME}.deb")
    elseif(NOT RPMCAPABLE MATCHES "NOTFOUND")
        set(CPACK_GENERATOR "RPM")
        set(INSTALL_LIB_DIR
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu)
        set(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
        message("-- Package name: ${CPACK_PACKAGE_FILE_NAME}.rpm")
endif()
endif()
