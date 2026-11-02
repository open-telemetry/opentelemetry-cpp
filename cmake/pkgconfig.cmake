# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

# Unlike functions, macros do not introduce a scope. This is an advantage when
# trying to set global variables, as we do here.
macro (opentelemetry_set_pkgconfig_paths)
    if (IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
      set(OPENTELEMETRY_PC_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
    else ()
      set(OPENTELEMETRY_PC_LIBDIR
            "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
    endif ()

    if (IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
      set(OPENTELEMETRY_PC_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
    else ()
      set(OPENTELEMETRY_PC_INCLUDEDIR
            "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
    endif ()
endmacro ()

# Create the pkgconfig configuration file (aka *.pc files) and the rules to install it.
#
# * library:     the short name of the target, such as `api` or `resources`.
# * name:        the displayed name of the library, such as "OpenTelemetry API".
# * description: the description of the library.
# * ARGN:        the names of any pkgconfig modules the generated module depends on.
# * EXTRA_LIBS:  optional extra `-l` names for dependencies with no usable
#                pkg-config module. Must come last. Each name is emitted as
#                `-l<lib>` in Libs.
#
function (opentelemetry_add_pkgconfig library name description)
  cmake_parse_arguments(pkgcfg "" "" "EXTRA_LIBS" ${ARGN})
  opentelemetry_set_pkgconfig_paths()
  set(target "opentelemetry_${library}")
  set(OPENTELEMETRY_PC_NAME "${name}")
  set(OPENTELEMETRY_PC_DESCRIPTION ${description})
  string(JOIN " " OPENTELEMETRY_PC_REQUIRES ${pkgcfg_UNPARSED_ARGUMENTS})
  get_target_property(target_type ${target} TYPE)
  if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
    # Interface libraries only contain headers. They do not generate lib files
    # to link against with `-l`.
    set(OPENTELEMETRY_PC_LIBS "")
  else ()
    set(OPENTELEMETRY_PC_LIBS "-l${target}")
  endif ()
  foreach (_extra_lib IN LISTS pkgcfg_EXTRA_LIBS)
    string(APPEND OPENTELEMETRY_PC_LIBS " -l${_extra_lib}")
  endforeach ()
  get_target_property(target_defs ${target} INTERFACE_COMPILE_DEFINITIONS)
  if (target_defs)
    foreach (def ${target_defs})
      string(REPLACE "\"" "\\\"" def "${def}")
      string(APPEND OPENTELEMETRY_PC_CFLAGS " -D${def}")
    endforeach ()
  endif ()

  # Create and install the pkg-config files.
  configure_file("${PROJECT_SOURCE_DIR}/cmake/templates/config.pc.in" "${target}.pc" @ONLY)
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endfunction()
