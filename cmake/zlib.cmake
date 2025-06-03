# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(DEFINED gRPC_PROVIDER AND NOT gRPC_PROVIDER STREQUAL "package" AND (TARGET zlib OR TARGET zlibstatic))
  # zlib was built as a submodule of gRPC
  set(ZLIB_PROVIDER "grpc_submodule")
  otel_add_thirdparty_package(
    PACKAGE_NAME "ZLIB"
    VERSION_REGEX "set\\([ \t]*VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?)\"\\)"
    VERSION_FILE "\${grpc_SOURCE_DIR}/third_party/zlib/CMakeLists.txt"
  )
else()
  if(${OPENTELEMETRY_INSTALL})
    set(_SKIP_ZLIB_INSTALL OFF)
  else()
    set(_SKIP_ZLIB_INSTALL ON)
  endif()

  otel_add_thirdparty_package(
    PACKAGE_NAME "ZLIB"
    PACKAGE_SEARCH_MODES "MODULE" "CONFIG"
    FETCH_NAME "zlib"
    FETCH_GIT_REPOSITORY "https://github.com/madler/zlib.git"
    FETCH_GIT_TAG "${zlib_GIT_TAG}"
    FETCH_CMAKE_ARGS
      ZLIB_BUILD_EXAMPLES=OFF
      ZLIB_INSTALL=${OPENTELEMETRY_INSTALL}
      SKIP_INSTALL_ALL=${_SKIP_ZLIB_INSTALL}
      SKIP_INSTALL_LIBRARIES=${_SKIP_ZLIB_INSTALL}
    VERSION_REGEX "set\\([ \t]*VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?)\"\\)"
    VERSION_FILE "\${zlib_SOURCE_DIR}/CMakeLists.txt"
  )
  unset(_SKIP_ZLIB_INSTALL)
endif()

if(TARGET zlib OR TARGET zlibstatic)
  # ZLIB_USE_STATIC_LIBS is added to FindZLIB with CMake 3.24 and defaults to OFF
  if(NOT DEFINED ZLIB_USE_STATIC_LIBS)
    set(ZLIB_USE_STATIC_LIBS OFF)
  endif()

  set(_ZLIB_TARGETS)
  if(TARGET zlib)
    set_target_properties(zlib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
    set_target_properties(zlib PROPERTIES EXPORT_NAME ZLIB)
    target_include_directories(zlib INTERFACE
      $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>
      $<BUILD_INTERFACE:${zlib_BINARY_DIR}>
      $<INSTALL_INTERFACE:include>)
    list(APPEND _ZLIB_TARGETS zlib)
  endif()

  if(TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")
    set_target_properties(zlibstatic PROPERTIES EXPORT_NAME ZLIBSTATIC)
    target_include_directories(zlibstatic INTERFACE
      $<BUILD_INTERFACE:${zlib_SOURCE_DIR}>
      $<BUILD_INTERFACE:${zlib_BINARY_DIR}>
      $<INSTALL_INTERFACE:include>)
    list(APPEND _ZLIB_TARGETS zlibstatic)
  endif()

  # Create an alias target for ZLIB::ZLIB
  if(NOT TARGET ZLIB::ZLIB)
    if(TARGET zlib AND NOT ZLIB_USE_STATIC_LIBS)
      add_library(ZLIB::ZLIB ALIAS zlib)
    elseif(TARGET zlibstatic AND ZLIB_USE_STATIC_LIBS)
      add_library(ZLIB::ZLIB ALIAS zlibstatic)
    endif()
  endif()

  if(OPENTELEMETRY_INSTALL)
    # ZLIB doesn't install its targets to an export set. This causes errors when trying to install opentelemetry-cpp targets that link to zlib.
    # Create an export set here for the zlib targets to be installed.
    # A config.cmake file is not created as we will still rely on the FindZLIB module to find zlib after install.
    install(
      TARGETS ${_ZLIB_TARGETS}
      EXPORT "zlib-target"
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT "zlib")

    install(
      EXPORT "zlib-target"
      FILE "zlib-target.cmake"
      NAMESPACE "ZLIB::"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ZLIB"
      COMPONENT "zlib")
  endif()
endif()

if(NOT TARGET ZLIB::ZLIB)
  message(FATAL_ERROR "A required zlib target (ZLIB::ZLIB) was not found")
endif()
