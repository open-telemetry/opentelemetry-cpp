# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

# Unlike functions, macros do not introduce a scope. This is an advantage when
# trying to set global variables, as we do here.
macro(opentelemetry_set_pkgconfig_paths)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(OPENTELEMETRY_PC_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
  else()
    set(OPENTELEMETRY_PC_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
  endif()

  if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(OPENTELEMETRY_PC_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
  else()
    set(OPENTELEMETRY_PC_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
  endif()
endmacro()

# Register pkg-config metadata on a target. Installation is performed later by
# _otel_install_target_pkgconfig() from _otel_install_component().
#
# * library:     the short name of the target, such as `api` or `resources`.
# * name:        the displayed name of the library, such as "OpenTelemetry API".
# * description: the description of the library.
# * ARGN:        the names of any pkgconfig modules the generated module depends on.
#
function(opentelemetry_add_pkgconfig library name description)
  set(_target "opentelemetry_${library}")

  if(NOT TARGET "${_target}")
    message(FATAL_ERROR
            "opentelemetry_add_pkgconfig: target ${_target} not found")
  endif()

  set_target_properties(
    "${_target}"
    PROPERTIES INTERFACE_OTEL_PKGCONFIG_SHORT_NAME "${library}"
               INTERFACE_OTEL_PKGCONFIG_NAME "${name}"
               INTERFACE_OTEL_PKGCONFIG_DESCRIPTION "${description}"
               INTERFACE_OTEL_PKGCONFIG_REQUIRES "${ARGN}")
endfunction()

# Generate and install a pkg-config file for a target that registered metadata
# via opentelemetry_add_pkgconfig().
function(_otel_install_target_pkgconfig _TARGET _COMPONENT)
  get_target_property(_short_name "${_TARGET}" INTERFACE_OTEL_PKGCONFIG_SHORT_NAME)
  if(NOT _short_name OR _short_name STREQUAL "_short_name-NOTFOUND")
    return()
  endif()

  get_target_property(_pc_name "${_TARGET}" INTERFACE_OTEL_PKGCONFIG_NAME)
  get_target_property(_pc_desc "${_TARGET}" INTERFACE_OTEL_PKGCONFIG_DESCRIPTION)
  get_target_property(_pc_requires "${_TARGET}" INTERFACE_OTEL_PKGCONFIG_REQUIRES)

  opentelemetry_set_pkgconfig_paths()

  set(OPENTELEMETRY_PC_NAME "${_pc_name}")
  set(OPENTELEMETRY_PC_DESCRIPTION "${_pc_desc}")
  string(JOIN " " OPENTELEMETRY_PC_REQUIRES ${_pc_requires})

  get_target_property(_target_type "${_TARGET}" TYPE)
  if("${_target_type}" STREQUAL "INTERFACE_LIBRARY")
    # Interface libraries only contain headers. They do not generate lib files
    # to link against with `-l`.
    set(OPENTELEMETRY_PC_LIBS "")
  else()
    set(OPENTELEMETRY_PC_LIBS "-lopentelemetry_${_short_name}")
  endif()

  set(OPENTELEMETRY_PC_CFLAGS "")
  get_target_property(_target_defs "${_TARGET}" INTERFACE_COMPILE_DEFINITIONS)
  if(_target_defs AND NOT _target_defs STREQUAL "_target_defs-NOTFOUND")
    foreach(_def IN LISTS _target_defs)
      string(APPEND OPENTELEMETRY_PC_CFLAGS " -D${_def}")
    endforeach()
  endif()

  set(_target "opentelemetry_${_short_name}")
  set(_pc_file "${PROJECT_BINARY_DIR}/pkgconfig/${_target}.pc")
  file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/pkgconfig")

  configure_file("${PROJECT_SOURCE_DIR}/cmake/templates/config.pc.in" "${_pc_file}"
                 @ONLY)

  install(
    FILES "${_pc_file}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT "${_COMPONENT}")
endfunction()
