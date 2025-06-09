# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include(GNUInstallDirs)
include(FetchContent)


# Initialize properties used below
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENTS_LIST "")
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_THIRDPARTY_AVAILABLE_LIST "")
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_THIRDPARTY_USED_LIST "")

########################################################################
# INTERNAL FUNCTIONS - do not call directly. Use the otel_* "Main" functions
########################################################################

#-----------------------------------------------------------------------
# _otel_set_component_properties:
#   Sets the component properties used for install.
#   Properties set on PROJECT_SOURCE_DIR directory include:
#     OTEL_COMPONENTS_LIST: List of components added using otel_add_component
#     OTEL_COMPONENT_TARGETS_<component>: List of targets associated with the component
#     OTEL_COMPONENT_TARGETS_ALIAS_<component>: List of targets aliases associated with the component
#     OTEL_COMPONENT_FILES_DIRECTORY_<component>: Directory containing files to be installed with the component
#     OTEL_COMPONENT_FILES_DESTINATION_<component>: Destination directory for the files
#     OTEL_COMPONENT_FILES_MATCHING_<component>: Matching pattern for the files to be installed
#     OTEL_COMPONENT_DEPENDS_<component>: List of components that this component depends on
#     OTEL_COMPONENT_THIRDPARTY_DEPENDS_<component>: List of thirdparty dependencies that this component depends on
#-----------------------------------------------------------------------
function(_otel_set_component_properties)
    set(optionArgs )
    set(oneValueArgs COMPONENT FILES_DIRECTORY FILES_DESTINATION)
    set(multiValueArgs TARGETS TARGETS_ALIAS FILES_MATCHING COMPONENT_DEPENDS THIRDPARTY_DEPENDS)
    cmake_parse_arguments(_PROPERTIES "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    # Add the component to the current components list
    get_property(existing_components DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENTS_LIST)
    if(_PROPERTIES_COMPONENT IN_LIST existing_components)
      message(FATAL_ERROR "  component ${_PROPERTIES_COMPONENT} has already been created.")
    endif()
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} APPEND PROPERTY OTEL_COMPONENTS_LIST "${_PROPERTIES_COMPONENT}")

    # Set the component targets property
    if(_PROPERTIES_TARGETS)
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_TARGETS_${_PROPERTIES_COMPONENT} "${_PROPERTIES_TARGETS}")
    else()
      message(FATAL_ERROR "  component ${_PROPERTIES_COMPONENT} does not have any targets.")
    endif()

    # Set the component targets alias property
    if(_PROPERTIES_TARGETS_ALIAS)
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_TARGETS_ALIAS_${_PROPERTIES_COMPONENT} "${_PROPERTIES_TARGETS_ALIAS}")
    endif()

    # Set the component files property
    if(_PROPERTIES_FILES_DIRECTORY)
      if(NOT _PROPERTIES_FILES_DESTINATION)
        message(FATAL_ERROR "  component ${_PROPERTIES_COMPONENT} has FILES_DIRECTORY set and must have FILES_DESINATION set.")
      endif()
      if(NOT _PROPERTIES_FILES_MATCHING)
        message(FATAL_ERROR "  component ${_PROPERTIES_COMPONENT} has FILES_DIRECTORY set and must have FILES_MATCHING set.")
      endif()

      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_DIRECTORY_${_PROPERTIES_COMPONENT} "${_PROPERTIES_FILES_DIRECTORY}")
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_DESTINATION_${_PROPERTIES_COMPONENT} "${_PROPERTIES_FILES_DESTINATION}")
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_MATCHING_${_PROPERTIES_COMPONENT} "${_PROPERTIES_FILES_MATCHING}")
    endif()

    if(_PROPERTIES_COMPONENT_DEPENDS)
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_DEPENDS_${_PROPERTIES_COMPONENT} "${_PROPERTIES_COMPONENT_DEPENDS}")
    endif()

    if(DEFINED _PROPERTIES_THIRDPARTY_DEPENDS)
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_THIRDPARTY_DEPENDS_${_PROPERTIES_COMPONENT} "${_PROPERTIES_THIRDPARTY_DEPENDS}")
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} APPEND PROPERTY OTEL_THIRDPARTY_USED_LIST "${_PROPERTIES_THIRDPARTY_DEPENDS}")
    endif()
endfunction()

#-----------------------------------------------------------------------
# _otel_set_target_component_property:
#   Sets the target's INTERFACE_OTEL_COMPONENT_NAME property to the component.
#   A target can only be assigned to one component.
# Note: The INTERFACE_* prefix can be dropped with CMake 3.19+ when custom
#       properties without the prefix are supported on INTERFACE targets.
#-----------------------------------------------------------------------
function(_otel_set_target_component_property _TARGET _COMPONENT)
  get_target_property(_TARGET_COMPONENT ${_TARGET} INTERFACE_OTEL_COMPONENT_NAME)
  if(_TARGET_COMPONENT)
    message(FATAL_ERROR "  Target ${_TARGET} is already assigned to an opentelemetry-cpp COMPONENT ${_TARGET_COMPONENT}.")
  endif()
  set_target_properties(${_TARGET} PROPERTIES INTERFACE_OTEL_COMPONENT_NAME ${_OTEL_ADD_COMP_COMPONENT})
endfunction()

#-----------------------------------------------------------------------
# _otel_append_dependent_components:
#   Appends the dependent component to the OUT_COMPONENTS variable.
#   The dependent component is defined in the OTEL_COMPONENT_DEPENDS_<component> property
#   on the PROJECT_SOURCE_DIR directory.
#-----------------------------------------------------------------------
function(_otel_append_dependent_components _COMPONENT OUT_COMPONENTS)
  get_property(_COMPONENT_DEPENDS DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_DEPENDS_${_COMPONENT})
  if(_COMPONENT_DEPENDS)
    set(_output_components "${${OUT_COMPONENTS}}")
    message(DEBUG "  - adding dependent component ${_COMPONENT_DEPENDS} from component ${_COMPONENT}")
    list(APPEND _output_components ${_COMPONENT_DEPENDS})
    set(${OUT_COMPONENTS} "${_output_components}" PARENT_SCOPE)
  endif()
endfunction()

#-----------------------------------------------------------------------
# _otel_append_component_found:
#   Checks if the target is associated with an otel component using the INTERFACE_OTEL_COMPONENT_NAME target property.
#   If so then the component is appended to the OUT_COMPONENTS varaiable.
#-----------------------------------------------------------------------
function(_otel_append_component_found _COMPONENT _TARGET OUT_COMPONENTS OUT_COMPONENT_FOUND)
  set(_output_components "${${OUT_COMPONENTS}}")
  get_target_property(_DEPEND_COMPONENT ${_TARGET} INTERFACE_OTEL_COMPONENT_NAME)
  if(_DEPEND_COMPONENT AND NOT ${_DEPEND_COMPONENT} STREQUAL ${_COMPONENT})
    _otel_append_dependent_components(${_DEPEND_COMPONENT} _output_components)
    message(DEBUG "  - adding dependent component ${_DEPEND_COMPONENT} from target ${_TARGET}")
    list(APPEND _output_components ${_DEPEND_COMPONENT})
    set(${OUT_COMPONENT_FOUND} TRUE PARENT_SCOPE)
  else()
    set(${OUT_COMPONENT_FOUND} FALSE PARENT_SCOPE)
  endif()
  set(${OUT_COMPONENTS} "${_output_components}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_append_thirdparty_found:
#   Tries to match one of the supported third-party dependencies to the target name.
#   If found the thirdparty package name is appended to the OUT_THIRDPARTY_DEPS variable.
#   The match is based on the OTEL_THIRDPARTY_AVAILABLE_LIST and
#   OTEL_<package>_NAMESPACE properties.
#------------------------------------------------------------------------
function(_otel_append_thirdparty_found _TARGET OUT_THIRDPARTY_DEPS)
  get_property(_DEPENDENCIES_LIST DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_THIRDPARTY_AVAILABLE_LIST)
  set(_output_thirdparty_deps "${${OUT_THIRDPARTY_DEPS}}")
  foreach(_DEPENDENCY ${_DEPENDENCIES_LIST})
    get_property(_DEPENDENCY_NAMESPACE DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_NAMESPACE)
    if(NOT DEFINED _DEPENDENCY_NAMESPACE)
      message(FATAL_ERROR "  OTEL_${_DEPENDENCY}_NAMESPACE property is not defined for ${_DEPENDENCY}.")
    endif()
    # Search for the dependency namespace in the target name
    string(FIND "${_TARGET}" "${_DEPENDENCY_NAMESPACE}" _is_thirdparty)
    if(_is_thirdparty GREATER -1)
      message(DEBUG "  - adding thirdparty dependency ${_DEPENDENCY} from target ${_TARGET}")
      list(APPEND _output_thirdparty_deps ${_DEPENDENCY})
    endif()
  endforeach()
  set(${OUT_THIRDPARTY_DEPS} "${_output_thirdparty_deps}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_collect_component_dependencies:
#   Collects the component to component dependencies and thirdparty dependencies of a target.
#   The dependencies are collected from the target's LINK_LIBRARIES property# and are appended
#   to the OUT_COMPONENT_DEPS and OUT_THIRDPARTY_DEPS variables.
#------------------------------------------------------------------------
function(_otel_collect_component_dependencies _TARGET _COMPONENT OUT_COMPONENT_DEPS OUT_THIRDPARTY_DEPS)
  get_target_property(_TARGET_TYPE ${_TARGET} TYPE)
  message(DEBUG "  Target: ${_TARGET} - Type: ${_TARGET_TYPE}")

  # Set the linked libraries to search for dependencies
  set(_linked_libraries "")
  if(_TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    get_target_property(_interface_libs ${_TARGET} INTERFACE_LINK_LIBRARIES)
    set(_linked_libraries "${_interface_libs}")
    message(DEBUG "  - INTERFACE_LINK_LIBRARIES: ${_interface_libs}")
  else()
    get_target_property(_link_libs ${_TARGET} LINK_LIBRARIES)
    set(_linked_libraries "${_link_libs}")
    message(DEBUG "  - LINK_LIBRARIES: ${_link_libs}")
  endif()

  set(_component_deps "${${OUT_COMPONENT_DEPS}}")
  set(_thirdparty_deps "${${OUT_THIRDPARTY_DEPS}}")

  foreach(_linked_target ${_linked_libraries})
    # Handle targets
    if(TARGET "${_linked_target}")
      set(_component_found FALSE)
      _otel_append_component_found(${_COMPONENT} "${_linked_target}" _component_deps _component_found)
      if(NOT ${_component_found})
        _otel_append_thirdparty_found(${_linked_target} _thirdparty_deps)
      endif()
      continue()
    endif()

    # Skip BUILD_INTERFACE targets
    string(FIND "${_linked_target}" "$<BUILD_INTERFACE:" _is_build_interface)
    if(_is_build_interface GREATER -1)
      message(DEBUG "  - skipping BUILD_INTERFACE target: ${_linked_target}")
      continue()
    endif()

    # Handle targets in generator expressions
    string(FIND "${_linked_target}" "$<" _is_generator_expression)
    if(_is_generator_expression GREATER -1)
      # Find targets in generator expressions (there can be more than one per expression)
      string(REGEX MATCHALL "[A-Za-z0-9_\\-\\.]+(::[A-Za-z0-9_\\-\\.]+)*" _parsed_targets "${_linked_target}")
      foreach(_parsed_target ${_parsed_targets})
        if(TARGET ${_parsed_target})
          set(_component_found FALSE)
          _otel_append_component_found(${_COMPONENT} "${_parsed_target}" _component_deps _component_found)
          if(NOT ${_component_found})
            _otel_append_thirdparty_found(${_parsed_target} _thirdparty_deps)
          endif()
        endif()
      endforeach()
    endif()
  endforeach()

  list(REMOVE_DUPLICATES _component_deps)
  list(REMOVE_DUPLICATES _thirdparty_deps)

  set(${OUT_COMPONENT_DEPS} "${_component_deps}" PARENT_SCOPE)
  set(${OUT_THIRDPARTY_DEPS} "${_thirdparty_deps}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_add_target_alias:
#   Adds an alias target using target's export name and project name.
#   opentelemetry-cpp::<target_export_name>
#------------------------------------------------------------------------
function(_otel_add_target_alias _TARGET OUT_ALIAS_TARGETS)
  get_target_property(_TARGET_EXPORT_NAME ${_TARGET} EXPORT_NAME)
  if(NOT _TARGET_EXPORT_NAME)
    message(FATAL_ERROR "  Target ${_TARGET} does not have an EXPORT_NAME property.")
  elseif(NOT TARGET "${PROJECT_NAME}::${_TARGET_EXPORT_NAME}")
    add_library("${PROJECT_NAME}::${_TARGET_EXPORT_NAME}" ALIAS ${_TARGET})
  endif()
  set(_alias_targets "${${OUT_ALIAS_TARGETS}}")
  list(APPEND _alias_targets "${PROJECT_NAME}::${_TARGET_EXPORT_NAME}")
  set(${OUT_ALIAS_TARGETS} "${_alias_targets}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_install_component:
#   Installs the component targets and optional files
#-----------------------------------------------------------------------
function(_otel_install_component _COMPONENT)
  install(
    TARGETS ${_COMPONENT_TARGETS}
    EXPORT "${PROJECT_NAME}-${_COMPONENT}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${_COMPONENT})

  install(
    EXPORT "${PROJECT_NAME}-${_COMPONENT}-target"
    FILE "${PROJECT_NAME}-${_COMPONENT}-target.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT ${_COMPONENT})

  if(_COMPONENT_FILES_DIRECTORY)
    install(
      DIRECTORY ${_COMPONENT_FILES_DIRECTORY}
      DESTINATION ${_COMPONENT_FILES_DESTINATION}
      COMPONENT ${_COMPONENT}
      FILES_MATCHING ${_COMPONENT_FILES_MATCHING})
  endif()
endfunction()

#-----------------------------------------------------------------------
# _otel_populate_component_targets_block:
#   Populates the OTEL_COMPONENTS_TARGETS_BLOCK with the component targets
#   - sets COMPONENT_<component>_TARGETS variables
#-----------------------------------------------------------------------
function(_otel_populate_component_targets_block IN_COMPONENT COMPONENTS_TARGETS_BLOCK)
  # Populate OTEL_COMPONENTS_TARGETS_BLOCK
  set(_targets_block ${${COMPONENTS_TARGETS_BLOCK}})
  string(APPEND _targets_block
    "# COMPONENT ${IN_COMPONENT}\n"
    "set(COMPONENT_${IN_COMPONENT}_TARGETS\n"
  )
  foreach(_TARGET IN LISTS _COMPONENT_TARGETS_ALIAS)
    string(APPEND _targets_block "    ${_TARGET}\n")
  endforeach()
  string(APPEND _targets_block ")\n\n")
  set(${COMPONENTS_TARGETS_BLOCK} "${_targets_block}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_populate_component_internal_depends_block:
#   Populates the OTEL_COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK with the component dependencies
#   - sets COMPONENT_<component>_COMPONENT_DEPENDS variables
#-----------------------------------------------------------------------
function(_otel_populate_component_internal_depends_block IN_COMPONENT COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK)
  # Populate OTEL_COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK
  set(_deps_block ${${COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK}})
  string(APPEND _deps_block
      "# COMPONENT ${IN_COMPONENT} internal dependencies\n"
      "set(COMPONENT_${IN_COMPONENT}_COMPONENT_DEPENDS\n"
    )
  foreach(dep IN LISTS _COMPONENT_DEPENDS)
    string(APPEND _deps_block "    ${dep}\n")
  endforeach()
  string(APPEND _deps_block ")\n\n")
  set(${COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK} "${_deps_block}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
function(_otel_populate_component_thirdparty_depends_block IN_COMPONENT COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK)
  # Populate OTEL_COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK
  set(_deps_block ${${COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK}})
  string(APPEND _deps_block
      "# COMPONENT ${IN_COMPONENT} thirdparty dependencies\n"
      "set(COMPONENT_${IN_COMPONENT}_THIRDPARTY_DEPENDS\n"
    )
  foreach(dep IN LISTS _COMPONENT_THIRDPARTY_DEPENDS)
    string(APPEND _deps_block "    ${dep}\n")
  endforeach()
  string(APPEND _deps_block ")\n\n")
  set(${COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK} "${_deps_block}" PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
function(_otel_save_cache_variables IN_VARIABLES)
  foreach (_variable IN LISTS ${IN_VARIABLES})
    # Save the variable to the project source directory property
    if(DEFINED CACHE{${_variable}})
      set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_SAVED_VAR_${_variable} "${${_variable}}")
      message(STATUS "  Saved variable ${_variable} to cache: ${${_variable}}")
    endif()
  endforeach()
endfunction()

#-----------------------------------------------------------------------
function(_otel_restore_cache_variables IN_VARIABLES)
  foreach (_variable IN LISTS ${IN_VARIABLES})
    # Restore the variable from the cache
    get_property(_saved_variable DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_SAVED_VAR_${_variable})
    if(DEFINED _saved_variable)
      set(${_variable} "${_saved_variable}" CACHE STRING "" FORCE)
      message(STATUS "  Restored variable ${_variable} to cache: ${_saved_variable}")
    elseif(DEFINED CACHE{${_variable}})
      unset(${_variable} CACHE)
      message(STATUS "  Restored variable ${_variable} to unset in cache")
    endif()
  endforeach()
endfunction()

#-----------------------------------------------------------------------
# _otel_find_package
#   Finds a third-party package using the provided search modes.
#  Arguments: (from the scope of otel_add_thirdparty_package)
#    _THIRDPARTY_PACKAGE_NAME: The name of the package to find
#    _THIRDPARTY_PACKAGE_SEARCH_MODES: The search modes to use for finding the package (e.g., "MODULE", "CONFIG")
#  Sets output variables at the PARENT_SCOPE:
#    Sets the <package>_FOUND variable to TRUE if the package is found and FALSE if not.
#    Sets the <package>_VERSION variable to the version of the package found.
#    Sets the <package>_SEARCH_MODE_SELECTED variable to the search mode used to find the package.
#    Sets the <package>_PROVIDER variable to "package" if the package is found.
function(_otel_find_package)

  foreach(_SEARCH_MODE IN LISTS _THIRDPARTY_PACKAGE_SEARCH_MODES)
    message(STATUS "  Searching for ${_THIRDPARTY_PACKAGE_NAME} with search mode ${_SEARCH_MODE}")
    find_package(${_THIRDPARTY_PACKAGE_NAME} ${_SEARCH_MODE} QUIET)
    if(${_THIRDPARTY_PACKAGE_NAME}_FOUND)
      set("${_THIRDPARTY_PACKAGE_NAME}_SEARCH_MODE_SELECTED" ${_SEARCH_MODE} PARENT_SCOPE)
      break()
    endif()
  endforeach()

  if(${_THIRDPARTY_PACKAGE_NAME}_FOUND)
    # Set <package>_VERSION from <package>_VERSION_STRING for legacy support (See FindCURL and FindZLIB CMake modules)
    if(DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION_STRING AND NOT DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION)
      set("${_THIRDPARTY_PACKAGE_NAME}_VERSION" "${${_THIRDPARTY_PACKAGE_NAME}_VERSION_STRING}")
    endif()
    set("${_THIRDPARTY_PACKAGE_NAME}_VERSION" ${${_THIRDPARTY_PACKAGE_NAME}_VERSION} PARENT_SCOPE)

    set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "package" PARENT_SCOPE)
  endif()

  set("${_THIRDPARTY_PACKAGE_NAME}_FOUND" ${${_THIRDPARTY_PACKAGE_NAME}_FOUND} PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# _otel_fetch_content
#   Use FetchContent to make a package available
#  Arguments: (from the scope of otel_add_thirdparty_package and prefixed with _THIRDPARTY_)
#    _THIRDPARTY_PACKAGE_NAME: The name of the package to fetch
#    _THIRDPARTY_FETCH_NAME: The name of the package to fetch
#    _THIRDPARTY_FETCH_SOURCE_DIR: The directory to fetch the package into
#    _THIRDPARTY_FETCH_GIT_REPOSITORY: The git repository URL to fetch the package from
#    _THIRDPARTY_FETCH_GIT_TAG: The git tag to checkout
#    _THIRDPARTY_FETCH_CMAKE_ARGS: The cmake arguments used to build the package. These are force cached variables. Be careful as they override any existing values and persist.
#  Sets output variables at the PARENT_SCOPE:
#    <package>_SOURCE_DIR variable to the source directory of the fetched package.
#    <package>_BINARY_DIR variable to the binary directory of the fetched package.
#    <package>_POPULATED variable to TRUE if the package is populated, FALSE otherwise.
function(_otel_fetch_content)

  if(DEFINED _THIRDPARTY_FETCH_SOURCE_DIR AND EXISTS "${_THIRDPARTY_FETCH_SOURCE_DIR}/.git")
    message(STATUS "  FetchContent_Declare ${_THIRDPARTY_FETCH_PACKAGE_NAME} from local source at ${_THIRDPARTY_FETCH_SOURCE_DIR}")
    FetchContent_Declare(
        ${_THIRDPARTY_FETCH_NAME}
        SOURCE_DIR ${_THIRDPARTY_FETCH_SOURCE_DIR}
    )
    set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "fetch_source" PARENT_SCOPE)
  elseif( DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY AND DEFINED _THIRDPARTY_FETCH_GIT_TAG)
    message(STATUS "  FetchContent_Declare ${_THIRDPARTY_PACKAGE_NAME} from ${_THIRDPARTY_FETCH_GIT_REPOSITORY} at tag ${_THIRDPARTY_FETCH_GIT_TAG}")
    FetchContent_Declare(
        ${_THIRDPARTY_FETCH_NAME}
        GIT_REPOSITORY
        ${_THIRDPARTY_FETCH_GIT_REPOSITORY}
        GIT_TAG
        ${_THIRDPARTY_FETCH_GIT_TAG}
        GIT_SHALLOW ON
        GIT_SUBMODULES ${_THIRDPARTY_FETCH_GIT_SUBMODULES}
    )
    set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "fetch_repository" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "No valid source found for ${_THIRDPARTY_PACKAGE_NAME}")
  endif()

  # CMake project options must be set in the cache before FetchContent_MakeAvailable is called.
  # Save the previous cache values we don't want to override for the opentelemetry-cpp project build
  # so they can be restored after the third-party package is configured.
  set(_OTEL_CACHE_VARIABLES_TO_SAVE
    BUILD_SHARED_LIBS
    BUILD_TESTING
    ENABLE_TESTING
    CMAKE_CXX_CLANG_TIDY
    CMAKE_CXX_INCLUDE_WHAT_YOU_USE)

  _otel_save_cache_variables(_OTEL_CACHE_VARIABLES_TO_SAVE)

  FetchContent_GetProperties(${_THIRDPARTY_FETCH_NAME})
  if(NOT ${_THIRDPARTY_FETCH_NAME}_POPULATED)
    # The package has not been populated yet and will build in-tree.
    # We need to force cache cmake arguments to configure the package.

    # Disable clang tidy and include-what-you-use for the third-party package
    set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "" CACHE STRING "" FORCE)

    # Set the CMake cache variables (KEY=VALUE) for the third party package
    foreach(_arg IN LISTS _THIRDPARTY_FETCH_CMAKE_ARGS)
      if(_arg MATCHES "^-D([^=]+)=(.*)$")
        set(_key   "${CMAKE_MATCH_1}")
        set(_value "${CMAKE_MATCH_2}")
        message(DEBUG "  Setting ${_key}=${_value}")
        set(${_key} "${_value}" CACHE STRING "" FORCE)
      else()
        message(WARNING "  ignoring malformed CMAKE_ARG: ${_arg}")
      endif()
    endforeach()
  endif()

  FetchContent_MakeAvailable(${_THIRDPARTY_FETCH_NAME})
  _otel_restore_cache_variables(_OTEL_CACHE_VARIABLES_TO_SAVE)

  set("${_THIRDPARTY_FETCH_NAME}_SOURCE_DIR" ${${_THIRDPARTY_FETCH_NAME}_SOURCE_DIR} PARENT_SCOPE)
  set("${_THIRDPARTY_FETCH_NAME}_BINARY_DIR" ${${_THIRDPARTY_FETCH_NAME}_BINARY_DIR} PARENT_SCOPE)
  set("${_THIRDPARTY_FETCH_NAME}_POPULATED" ${${_THIRDPARTY_FETCH_NAME}_POPULATED} PARENT_SCOPE)
endfunction()

########################################################################
# Main functions to support installing components
# and the opentlemetry-cpp cmake package config files
########################################################################

#-------------------------------------------------------------------------------
# otel_add_thirdparty_package
# Add a third party package to the project using CMake's find_package or FetchContent.
# By default the dependencies are found/fetched in the following order:
#   Step 1. Search for installed packages by calling find_package using the provided search modes.
#   Step 2. Use FetchContent on the provided source directory (used for git submodules)
#   Step 3. Use FetchContent to fetch the package from a git repository if not found in the previous steps.
# Arguments:
#   PACKAGE_NAME: The name of the package to find or fetch
#   PACKAGE_NAMESPACE: The package install namespace. Defaults to PACKAGE_NAME.
#   PACKAGE_SEARCH_MODES: The cmake find_package search modes. Multiple search modes can be provided
#            (ie: SEARCH_MODES "MODULE" "CONFIG" will search first using the MODULE mode, then CONFIG mode).
#   FETCH_NAME: The name of the package to fetch if not found
#   FETCH_SOURCE_DIR: The directory to fetch the package into
#   FETCH_GIT_REPOSITORY: The git repository URL to fetch the package from
#   FETCH_GIT_TAG: The git tag to checkout
#   FETCH_CMAKE_ARGS: The cmake arguments used to build the package. These are force cached variables. Be careful as they override any existing values and persist.
#   REQUIRED_TARGETS: The targets to check for existence
#   VERSION_REGEX: The regex to parse the package version if fetched from source or git repository.
#   VERSION_FILE: The file to read the version from when using VERSION_REGEX.

# PROJECT_SOURCE_DIR directory properties set:
#   OTEL_THIRDPARTY_AVAILABLE_LIST: List of third-party packages available in the project
#   OTEL_<package>_NAMESPACE: The package's install namespace
#   OTEL_<package>_VERSION: The version of the package found or fetched
#   OTEL_<package>_PROVIDER: The provider of the package (package, fetch_source, fetch_repository)
#   OTEL_<package>_SEARCH_MODE: The search mode used to find the package
#   OTEL_<package>_SOURCE_DIR: Set if the package was fetched
#   OTEL_<package>_BINARY_DIR: Set if the package was fetched
#   OTEL_<package>_POPULATED: Set if the package was populated by FetchContent
function(otel_add_thirdparty_package)

  set(optionArgs ALWAYS_FETCH ALWAYS_FIND DO_NOT_FETCH DO_NOT_FIND)
  set(oneValueArgs PACKAGE_NAME PACKAGE_NAMESPACE FETCH_NAME FETCH_GIT_REPOSITORY FETCH_GIT_TAG FETCH_SOURCE_DIR VERSION_REGEX VERSION_FILE)
  set(multiValueArgs PACKAGE_SEARCH_MODES FETCH_GIT_SUBMODULES FETCH_CMAKE_ARGS REQUIRED_TARGETS )
  cmake_parse_arguments(_THIRDPARTY "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

  if(NOT _THIRDPARTY_PACKAGE_NAME)
    message(FATAL_ERROR "PACKAGE_NAME is required")
  endif()

  if(DEFINED _THIRDPARTY_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unparsed arguments detected: ${_THIRDPARTY_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT DEFINED _THIRDPARTY_PACKAGE_NAMESPACE)
    set(_THIRDPARTY_PACKAGE_NAMESPACE "${_THIRDPARTY_PACKAGE_NAME}")
  endif()

  if(DEFINED _THIRDPARTY_PACKAGE_SEARCH_MODES)
    list(GET _THIRDPARTY_PACKAGE_SEARCH_MODES 0 "${_THIRDPARTY_PACKAGE_NAME}_SEARCH_MODE_SELECTED")
  endif()

  if(NOT _THIRDPARTY_FETCH_NAME)
    set(_THIRDPARTY_FETCH_NAME ${_THIRDPARTY_PACKAGE_NAME})
  endif()

  if(DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY AND NOT _THIRDPARTY_FETCH_GIT_TAG)
    message(FATAL_ERROR "FETCH_GIT_TAG is required if FETCH_GIT_REPOSITORY is defined")
  endif()

  set(_FETCH_SUPPORTED FALSE)
  if((DEFINED _THIRDPARTY_FETCH_SOURCE_DIR OR DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY) AND NOT _THIRDPARTY_DO_NOT_FETCH)
    set(_FETCH_SUPPORTED TRUE)
  endif()

  set(_FIND_SUPPORTED FALSE)
  if(DEFINED _THIRDPARTY_PACKAGE_SEARCH_MODES AND NOT _THIRDPARTY_DO_NOT_FIND)
    set(_FIND_SUPPORTED TRUE)
  endif()

  set(_FIND_ALLOWED FALSE)
  if(NOT OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY OR _THIRDPARTY_ALWAYS_FIND)
    set(_FIND_ALLOWED TRUE)
  endif()

  set(_FETCH_ALLOWED FALSE)
  if(NOT OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY OR _THIRDPARTY_ALWAYS_FETCH)
    set(_FETCH_ALLOWED TRUE)
  endif()

  message(STATUS "Adding third party package ${_THIRDPARTY_PACKAGE_NAME}")
  message(DEBUG "  Search modes: ${_THIRDPARTY_PACKAGE_SEARCH_MODES}")
  message(DEBUG "  Fetch name: ${_THIRDPARTY_FETCH_NAME}")
  message(DEBUG "  Source dir: ${_THIRDPARTY_FETCH_SOURCE_DIR}")
  message(DEBUG "  Git repository: ${_THIRDPARTY_FETCH_GIT_REPOSITORY}")
  message(DEBUG "  Git tag: ${_THIRDPARTY_FETCH_GIT_TAG}")
  message(DEBUG "  Git submodules: ${_THIRDPARTY_FETCH_GIT_SUBMODULES}")
  message(DEBUG "  CMake args: ${_THIRDPARTY_FETCH_CMAKE_ARGS}")
  message(DEBUG "  Required targets: ${_THIRDPARTY_REQUIRED_TARGETS}")
  message(DEBUG "  Version regex: ${_THIRDPARTY_VERSION_REGEX}")
  message(DEBUG "  Version file: ${_THIRDPARTY_VERSION_FILE}")

  if(_FIND_SUPPORTED AND _FIND_ALLOWED)
    _otel_find_package()
  endif()

  if(NOT ${_THIRDPARTY_PACKAGE_NAME}_FOUND AND _FETCH_SUPPORTED AND _FETCH_ALLOWED)
    _otel_fetch_content()
  endif()

  # Check for required targets
  foreach(_target ${_THIRDPARTY_REQUIRED_TARGETS})
    if(NOT TARGET ${_target})
      message(FATAL_ERROR "Required target ${_target} not found")
    endif()
  endforeach()

  # Determine the package version if not already defined
  if(NOT DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION)
    if(${_THIRDPARTY_PACKAGE_NAME}_PROVIDER STREQUAL "package")
      set("${_THIRDPARTY_PACKAGE_NAME}_VERSION" "unknown")
    elseif(DEFINED _THIRDPARTY_VERSION_REGEX AND DEFINED _THIRDPARTY_VERSION_FILE)
      string(CONFIGURE "${_THIRDPARTY_VERSION_FILE}" _CONFIGURED_VERSION_FILEPATH)
      if(NOT EXISTS "${_CONFIGURED_VERSION_FILEPATH}")
        message(WARNING "Version file ${_CONFIGURED_VERSION_FILEPATH} does not exist")
      else()
        file(READ "${_CONFIGURED_VERSION_FILEPATH}" _VERSION_FILE_CONTENTS)
        if(_VERSION_FILE_CONTENTS MATCHES ${_THIRDPARTY_VERSION_REGEX})
          set("${_THIRDPARTY_PACKAGE_NAME}_VERSION" "${CMAKE_MATCH_1}")
        else()
          message(WARNING "Failed to parse version from ${_THIRDPARTY_VERSION_FILE} using regex ${_THIRDPARTY_VERSION_REGEX}")
        endif()
      endif()
    endif()
  endif()

  message(STATUS "  ${_THIRDPARTY_PACKAGE_NAME} version: ${${_THIRDPARTY_PACKAGE_NAME}_VERSION}")
  message(STATUS "  ${_THIRDPARTY_PACKAGE_NAME} provider: ${${_THIRDPARTY_PACKAGE_NAME}_PROVIDER}")

  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} APPEND PROPERTY "OTEL_THIRDPARTY_AVAILABLE_LIST" "${_THIRDPARTY_PACKAGE_NAME}")
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_VERSION" "${${_THIRDPARTY_PACKAGE_NAME}_VERSION}")
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "${${_THIRDPARTY_PACKAGE_NAME}_PROVIDER}")
  set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_NAMESPACE" "${_THIRDPARTY_PACKAGE_NAMESPACE}")
  if(DEFINED "${_THIRDPARTY_PACKAGE_NAME}_SEARCH_MODE_SELECTED")
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_SEARCH_MODE" "${${_THIRDPARTY_PACKAGE_NAME}_SEARCH_MODE_SELECTED}")
  endif()

  if(NOT ${_THIRDPARTY_PACKAGE_NAME}_PROVIDER STREQUAL "package")
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_SOURCE_DIR" "${${_THIRDPARTY_FETCH_NAME}_SOURCE_DIR}")
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_BINARY_DIR" "${${_THIRDPARTY_FETCH_NAME}_BINARY_DIR}")
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY "OTEL_${_THIRDPARTY_PACKAGE_NAME}_POPULATED" "${${_THIRDPARTY_FETCH_NAME}_POPULATED}")
  endif()
endfunction()

#-----------------------------------------------------------------------
# otel_add_component:
#   Adds a component to the list of components to be installed. A component name and list of targest are required.
#   Optional files can be added to the component by specifying a directory, destination and matching pattern.
#   Each target is assigned to the component and its dependencies are identified based on the LINK_LIBRARIES property.
#   An alias target is also created for each target in the form of PROJECT_NAME::TARGET_EXPORT_NAME.
# Usage:
#    otel_add_component(
#      COMPONENT <component_name>
#      TARGETS <target1> <target2> ...
#      FILES_DIRECTORY <directory>
#      FILES_DESTINATION <destination>
#      FILES_MATCHING <matching>)
#-----------------------------------------------------------------------
function(otel_add_component)
  set(optionArgs )
  set(oneValueArgs COMPONENT FILES_DIRECTORY FILES_DESTINATION)
  set(multiValueArgs TARGETS FILES_MATCHING)
  cmake_parse_arguments(_OTEL_ADD_COMP "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

  if(NOT _OTEL_ADD_COMP_COMPONENT)
    message(FATAL_ERROR "otel_add_component: COMPONENT is required")
  endif()

  if(NOT _OTEL_ADD_COMP_TARGETS)
    message(FATAL_ERROR "otel_add_component: TARGETS is required")
  endif()

  message(DEBUG "Add COMPONENT: ${_OTEL_ADD_COMP_COMPONENT}")
  set(_COMPONENT_DEPENDS "")
  set(_THIRDPARTY_DEPENDS "")
  set(_ALIAS_TARGETS "")

  foreach(_TARGET ${_OTEL_ADD_COMP_TARGETS})
    if(NOT TARGET ${_TARGET})
      message(FATAL_ERROR "  Target ${_TARGET} not found")
    endif()
    _otel_set_target_component_property(${_TARGET} ${_OTEL_ADD_COMP_COMPONENT})
    _otel_collect_component_dependencies(${_TARGET} ${_OTEL_ADD_COMP_COMPONENT} _COMPONENT_DEPENDS _THIRDPARTY_DEPENDS)
    _otel_add_target_alias(${_TARGET} _ALIAS_TARGETS)
  endforeach()

  if(_OTEL_ADD_COMP_FILES_DIRECTORY)
    set(_OTEL_ADD_COMP_FILES_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_OTEL_ADD_COMP_FILES_DIRECTORY}")
  endif()

  message(DEBUG "  TARGETS: ${_OTEL_ADD_COMP_TARGETS}")
  message(DEBUG "  TARGETS_ALIAS: ${_ALIAS_TARGETS}")
  message(DEBUG "  COMPONENT_DEPENDS: ${_COMPONENT_DEPENDS}")
  message(DEBUG "  THIRDPARTY_DEPENDS: ${_THIRDPARTY_DEPENDS}")
  message(DEBUG "  FILES_DIRECTORY: ${_OTEL_ADD_COMP_FILES_DIRECTORY}")
  message(DEBUG "  FILES_DESTINATION: ${_OTEL_ADD_COMP_FILES_DESTINATION}")
  message(DEBUG "  FILES_MATCHING: ${_OTEL_ADD_COMP_FILES_MATCHING}")

  _otel_set_component_properties(
    COMPONENT ${_OTEL_ADD_COMP_COMPONENT}
    TARGETS ${_OTEL_ADD_COMP_TARGETS}
    TARGETS_ALIAS ${_ALIAS_TARGETS}
    FILES_DIRECTORY ${_OTEL_ADD_COMP_FILES_DIRECTORY}
    FILES_DESTINATION ${_OTEL_ADD_COMP_FILES_DESTINATION}
    FILES_MATCHING ${_OTEL_ADD_COMP_FILES_MATCHING}
    COMPONENT_DEPENDS ${_COMPONENT_DEPENDS}
    THIRDPARTY_DEPENDS ${_THIRDPARTY_DEPENDS})
endfunction()

#-----------------------------------------------------------------------
# otel_install_components:
#   Installs all components that have been added using otel_add_component.
#   The components are installed in the order they were added.
#   The install function will create a cmake config file for each component
#   that contains the component name, targets, dependencies and thirdparty dependencies.
# Usage:
#    otel_install_components()
#-----------------------------------------------------------------------
function(otel_install_components)
  get_property(OTEL_BUILT_COMPONENTS_LIST DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENTS_LIST)
  message(STATUS "Installing components:")
  set(OTEL_COMPONENTS_TARGETS_BLOCK "")
  set(OTEL_COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK "")
  set(OTEL_COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK "")

  foreach(_COMPONENT ${OTEL_BUILT_COMPONENTS_LIST})
    get_property(_COMPONENT_DEPENDS DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_DEPENDS_${_COMPONENT})
    get_property(_COMPONENT_TARGETS DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_TARGETS_${_COMPONENT})
    get_property(_COMPONENT_TARGETS_ALIAS DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_TARGETS_ALIAS_${_COMPONENT})
    get_property(_COMPONENT_THIRDPARTY_DEPENDS DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_THIRDPARTY_DEPENDS_${_COMPONENT})
    get_property(_COMPONENT_FILES_DIRECTORY DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_DIRECTORY_${_COMPONENT})
    get_property(_COMPONENT_FILES_DESTINATION DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_DESTINATION_${_COMPONENT})
    get_property(_COMPONENT_FILES_MATCHING DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_COMPONENT_FILES_MATCHING_${_COMPONENT})

    message(STATUS "Install COMPONENT ${_COMPONENT}")
    message(STATUS "  TARGETS: ${_COMPONENT_TARGETS}")
    message(STATUS "  TARGETS_ALIAS: ${_COMPONENT_TARGETS_ALIAS}")
    message(STATUS "  COMPONENT_DEPENDS: ${_COMPONENT_DEPENDS}")
    message(STATUS "  THIRDPARTY_DEPENDS: ${_COMPONENT_THIRDPARTY_DEPENDS}")
    message(STATUS "  FILES_DIRECTORY: ${_COMPONENT_FILES_DIRECTORY}")
    message(STATUS "  FILES_DESTINATION: ${_COMPONENT_FILES_DESTINATION}")
    message(STATUS "  FILES_MATCHING: ${_COMPONENT_FILES_MATCHING}")

    _otel_install_component(${_COMPONENT})
    _otel_populate_component_targets_block(${_COMPONENT} OTEL_COMPONENTS_TARGETS_BLOCK)
    _otel_populate_component_internal_depends_block(${_COMPONENT} OTEL_COMPONENTS_INTERNAL_DEPENDENCIES_BLOCK)
    _otel_populate_component_thirdparty_depends_block(${_COMPONENT} OTEL_COMPONENTS_THIRDPARTY_DEPENDENCIES_BLOCK)
  endforeach()

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/templates/component-definitions.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/component-definitions.cmake"
    @ONLY
  )

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/component-definitions.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT cmake-config)
endfunction()

#-----------------------------------------------------------------------
# otel_install_thirdparty_definitions:
#   Installs the thirdparty dependency definitions file that contains the list
#   of thirdparty dependencies their versions and cmake search modes.
#   - sets `OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED` to the list of dependencies
#   - sets `OTEL_<dependency>_VERSION` to the version used to build opentelemetry-cpp
#   - sets `OTEL_<dependency>_SEARCH_MODE` to the search mode required to find the dependency
# Usage:
#    otel_install_thirdparty_definitions()
#-----------------------------------------------------------------------
function(otel_install_thirdparty_definitions)
  set(OTEL_THIRDPARTY_DEPENDENCY_VERSIONS_BLOCK "")
  set(OTEL_THIRDPARTY_DEPENDENCY_SEARCH_MODES_BLOCK "")

  get_property(OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_THIRDPARTY_USED_LIST)
  list(REMOVE_DUPLICATES OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED)

  # Populate OTEL_THIRDPARTY_DEPENDENCY_VERSIONS_BLOCK
  foreach(_DEPENDENCY IN LISTS OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED)
    get_property(_dependency_VERSION DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_VERSION)
    string(APPEND OTEL_THIRDPARTY_DEPENDENCY_VERSIONS_BLOCK
      "set(OTEL_${_DEPENDENCY}_VERSION \"${_dependency_VERSION}\")\n"
    )
  endforeach()

  # Populate OTEL_THIRDPARTY_DEPENDENCY_SEARCH_MODES_BLOCK
  foreach(_DEPENDENCY IN LISTS OTEL_THIRDPARTY_DEPENDENCIES_SUPPORTED)
    get_property(_dependency_SEARCH_MODE DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_SEARCH_MODE)
    string(APPEND OTEL_THIRDPARTY_DEPENDENCY_SEARCH_MODES_BLOCK
      "set(OTEL_${_DEPENDENCY}_SEARCH_MODE \"${_dependency_SEARCH_MODE}\")\n"
    )
  endforeach()

  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/templates/thirdparty-dependency-definitions.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/thirdparty-dependency-definitions.cmake"
    @ONLY)

  install(
      FILES
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/thirdparty-dependency-definitions.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
      COMPONENT cmake-config)
endfunction()

#-----------------------------------------------------------------------
# otel_install_cmake_config:
#   Configures and installs the cmake.config package file and version file
#   to support find_package(opentelemetry-cpp CONFIG COMPONENTS ...)
# Usage:
#    otel_install_cmake_config()
#-----------------------------------------------------------------------
function(otel_install_cmake_config)
  include(CMakePackageConfigHelpers)

  # Write config file for find_package(opentelemetry-cpp CONFIG)
  set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/templates/opentelemetry-cpp-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    PATH_VARS INCLUDE_INSTALL_DIR CMAKE_INSTALL_LIBDIR)

  # Write version file for find_package(opentelemetry-cpp CONFIG)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY ExactVersion)

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config-version.cmake"
      "${CMAKE_CURRENT_LIST_DIR}/cmake/find-package-support-functions.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT cmake-config)
endfunction()

#-----------------------------------------------------------------------
#  otel_print_build_config
#   Prints the build configuration and options used to build opentelemetry-cpp.
function(otel_print_build_config)
    # Record build config and versions
    message(STATUS "---------------------------------------------")
    message(STATUS "build settings")
    message(STATUS "---------------------------------------------")
    message(STATUS "OpenTelemetry: ${OPENTELEMETRY_VERSION}")
    message(STATUS "OpenTelemetry ABI: ${OPENTELEMETRY_ABI_VERSION_NO}")
    message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "CXX: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    message(STATUS "CXXFLAGS: ${CMAKE_CXX_FLAGS}")
    message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

    message(STATUS "---------------------------------------------")
    message(STATUS "opentelemetry-cpp build options")
    message(STATUS "---------------------------------------------")
    message(STATUS "WITH_API_ONLY: ${WITH_API_ONLY}")
    message(STATUS "WITH_NO_DEPRECATED_CODE: ${WITH_NO_DEPRECATED_CODE}")
    message(STATUS "WITH_ABI_VERSION_1: ${WITH_ABI_VERSION_1}")
    message(STATUS "WITH_ABI_VERSION_2: ${WITH_ABI_VERSION_2}")
    message(STATUS "OTELCPP_VERSIONED_LIBS: ${OTELCPP_VERSIONED_LIBS}")
    message(STATUS "OTELCPP_MAINTAINER_MODE: ${OTELCPP_MAINTAINER_MODE}")
    message(STATUS "WITH_STL: ${WITH_STL}")
    message(STATUS "WITH_GSL: ${WITH_GSL}")
    message(STATUS "WITH_NO_GETENV: ${WITH_NO_GETENV}")
    message(STATUS "OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY: ${OTELCPP_THIRDPARTY_FETCH_CONTENT_ONLY}")
    message(STATUS "OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY: ${OTELCPP_THIRDPARTY_FIND_PACKAGE_ONLY}")

    message(STATUS "---------------------------------------------")
    message(STATUS "opentelemetry-cpp cmake component options")
    message(STATUS "---------------------------------------------")
    message(STATUS "WITH_OTLP_GRPC: ${WITH_OTLP_GRPC}")
    message(STATUS "WITH_OTLP_HTTP: ${WITH_OTLP_HTTP}")
    message(STATUS "WITH_OTLP_FILE: ${WITH_OTLP_FILE}")
    message(STATUS "WITH_HTTP_CLIENT_CURL: ${WITH_HTTP_CLIENT_CURL}")
    message(STATUS "WITH_ZIPKIN: ${WITH_ZIPKIN}")
    message(STATUS "WITH_PROMETHEUS: ${WITH_PROMETHEUS}")
    message(STATUS "WITH_ELASTICSEARCH: ${WITH_ELASTICSEARCH}")
    message(STATUS "WITH_OPENTRACING: ${WITH_OPENTRACING}")
    message(STATUS "WITH_ETW: ${WITH_ETW}")
    message(STATUS "OPENTELEMETRY_BUILD_DLL: ${OPENTELEMETRY_BUILD_DLL}")

    message(STATUS "---------------------------------------------")
    message(STATUS "feature preview options")
    message(STATUS "---------------------------------------------")
    message(STATUS "WITH_ASYNC_EXPORT_PREVIEW: ${WITH_ASYNC_EXPORT_PREVIEW}")
    message(
      STATUS
        "WITH_THREAD_INSTRUMENTATION_PREVIEW: ${WITH_THREAD_INSTRUMENTATION_PREVIEW}"
    )
    message(
      STATUS "WITH_METRICS_EXEMPLAR_PREVIEW: ${WITH_METRICS_EXEMPLAR_PREVIEW}")
    message(STATUS "WITH_REMOVE_METER_PREVIEW: ${WITH_REMOVE_METER_PREVIEW}")
    message(
      STATUS "WITH_OTLP_GRPC_SSL_MTLS_PREVIEW: ${WITH_OTLP_GRPC_SSL_MTLS_PREVIEW}")
    message(STATUS "WITH_OTLP_GRPC_CREDENTIAL_PREVIEW: ${WITH_OTLP_GRPC_CREDENTIAL_PREVIEW}")
    message(STATUS "WITH_OTLP_RETRY_PREVIEW: ${WITH_OTLP_RETRY_PREVIEW}")
    message(STATUS "---------------------------------------------")
    message(STATUS "third-party options")
    message(STATUS "---------------------------------------------")
    message(STATUS "WITH_CURL_LOGGING: ${WITH_CURL_LOGGING}")
    message(STATUS "WITH_OTLP_HTTP_COMPRESSION: ${WITH_OTLP_HTTP_COMPRESSION}")
    message(STATUS "---------------------------------------------")
    message(STATUS "examples and test options")
    message(STATUS "---------------------------------------------")
    message(STATUS "WITH_BENCHMARK: ${WITH_BENCHMARK}")
    message(STATUS "WITH_EXAMPLES: ${WITH_EXAMPLES}")
    message(STATUS "WITH_EXAMPLES_HTTP: ${WITH_EXAMPLES_HTTP}")
    message(STATUS "WITH_FUNC_TESTS: ${WITH_FUNC_TESTS}")
    message(STATUS "BUILD_W3CTRACECONTEXT_TEST: ${BUILD_W3CTRACECONTEXT_TEST}")
    message(STATUS "BUILD_TESTING: ${BUILD_TESTING}")
    message(STATUS "---------------------------------------------")
    message(STATUS "versions")
    message(STATUS "---------------------------------------------")
    message(STATUS "CMake: ${CMAKE_VERSION}")

    get_property(_DEPENDENCIES_LIST DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_THIRDPARTY_AVAILABLE_LIST)
    foreach(_DEPENDENCY IN LISTS _DEPENDENCIES_LIST)
        get_property(_dependency_PROVIDER DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_PROVIDER)
        get_property(_dependency_VERSION DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_VERSION)
        get_property(_dependency_SEARCH_MODE DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY OTEL_${_DEPENDENCY}_SEARCH_MODE)
        if(DEFINED _dependency_SEARCH_MODE)
            set(_dependency_SEARCH_MODE ", ${_dependency_SEARCH_MODE}")
        else()
            set(_dependency_SEARCH_MODE "")
        endif()
        message(STATUS "${_DEPENDENCY}: ${_dependency_VERSION} (${_dependency_PROVIDER}${_dependency_SEARCH_MODE})")
    endforeach()
    message(STATUS "---------------------------------------------")
endfunction()
