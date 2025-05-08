# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

include("${CMAKE_CURRENT_LIST_DIR}/component-definitions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/thirdparty-dependency-definitions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/thirdparty-built-with-flags.cmake")

#-------------------------------------------------------------------------
# Functions to get supported, installed, and requested components.
#-------------------------------------------------------------------------
function(get_supported_components components_out)
  set(${components_out} ${opentelemetry-cpp_COMPONENTS} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Function to get installed components.
#-------------------------------------------------------------------------
function(get_installed_components installed_components_out)
  set(result "")
  foreach(_COMPONENT IN LISTS opentelemetry-cpp_COMPONENTS)
      set(_COMPONENT_TARGET_FILE "${CMAKE_CURRENT_LIST_DIR}/opentelemetry-cpp-${_COMPONENT}-target.cmake")
      if(EXISTS "${_COMPONENT_TARGET_FILE}")
          list(APPEND result ${_COMPONENT})
          message(DEBUG "get_installed_components: component = ${_COMPONENT},  installed = TRUE")
      else()
          message(DEBUG "get_installed_components: component = ${_COMPONENT},  installed = FALSE")
      endif()
  endforeach()
  set(${installed_components_out} ${result} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Function to get dependent components.
#-------------------------------------------------------------------------
function(get_dependent_components component_in dependent_components_out)
  set(result "")
  set(depends_var "COMPONENT_${component_in}_COMPONENT_DEPENDS")
  if(DEFINED ${depends_var})
    set(result ${${depends_var}})
  endif()
  set(${dependent_components_out} ${result} PARENT_SCOPE)
endfunction()


#-------------------------------------------------------------------------
# Function to get requested components.
#-------------------------------------------------------------------------
function(get_requested_components installed_components_in requested_components_out)
  set(result "")
  if (NOT opentelemetry-cpp_FIND_COMPONENTS)
    set(result ${${installed_components_in}})
    message(DEBUG "get_requested_components: No components explicitly requested. Importing all installed components including: ${result}")
    set(${requested_components_out} ${result} PARENT_SCOPE)
  else()
    message(DEBUG "get_requested_components: Components requested: ${opentelemetry-cpp_FIND_COMPONENTS}")
    foreach(_COMPONENT IN LISTS opentelemetry-cpp_FIND_COMPONENTS)
        if(NOT ${_COMPONENT} IN_LIST opentelemetry-cpp_COMPONENTS)
            message(ERROR " get_requested_components: Component `${_COMPONENT}` is not a supported component of the opentelemetry-cpp package. Supported components include: ${opentelemetry-cpp_COMPONENTS}")
            return()
        endif()
        if(NOT ${_COMPONENT} IN_LIST ${installed_components_in})
            message(FATAL_ERROR " get_requested_components: Component `${_COMPONENT}` is supported by opentelemetry-cpp but not installed. Installed components include: ${${installed_components_in}}")
        endif()
        get_dependent_components(${_COMPONENT} _DEPENDENT_COMPONENTS)
        foreach(_DEPENDENT_COMPONENT IN LISTS _DEPENDENT_COMPONENTS)
            if(NOT ${_DEPENDENT_COMPONENT} IN_LIST result)
                list(APPEND result ${_DEPENDENT_COMPONENT})
            endif()
        endforeach(_DEPENDENT_COMPONENT IN LISTS _DEPENDENT_COMPONENTS)
        if(NOT ${_COMPONENT} IN_LIST result)
            list(APPEND result ${_COMPONENT})
        endif()
    endforeach()
    set(${requested_components_out} ${result} PARENT_SCOPE)
  endif()
endfunction()


#-------------------------------------------------------------------------
# Function to get the targets for a component.
#-------------------------------------------------------------------------
function(get_component_targets component_in targets_out)
  set(result "")
  if(NOT ${comp} IN_LIST opentelemetry-cpp_COMPONENTS)
      message(ERROR " get_component_targets: Component `${comp}` component is not a supported component of the opentelemetry-cpp package.")
  else()
    set(targets_var "COMPONENT_opentelemetry-cpp_${comp}_TARGETS")
    if(DEFINED ${targets_var})
      set(result ${${targets_var}})
    else()
      message(FATAL_ERROR " get_component_targets: ${targets_var} is not defined. Please add it to component-definitions.cmake to define the exported targets for this component.")
    endif()
  endif()
  set(${targets_out} ${result} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Get targets for a list of components.
#-------------------------------------------------------------------------
function(get_targets components_in targets_out)
  set(result "")
  foreach(comp IN LISTS ${components_in})
    get_component_targets(${comp} comp_targets)
    foreach(target IN LISTS comp_targets)
      list(APPEND result ${target})
    endforeach()
  endforeach()
  set(${targets_out} ${result} PARENT_SCOPE)
  message(DEBUG "get_targets: found the following installed and requested targets. ${result}")
endfunction()


#-------------------------------------------------------------------------
# Check if a target is imported for a list of targets.
#-------------------------------------------------------------------------
function(check_targets_imported targets_in)
  set(result TRUE)
  foreach(target IN LISTS ${targets_in})
    if(TARGET ${target})
      message(DEBUG "check_targets_imported: imported target `${target}`")
    else()
      message(FATAL_ERROR " check_targets_imported: failed to import target `${target}`")
      set(result FALSE)
    endif()
  endforeach()
  set(${result_bool_out} ${result} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Function to get all supported third party dependencies
#-------------------------------------------------------------------------
function(get_supported_third_party_dependencies dependencies_out)
  set(${dependencies_out} ${THIRD_PARTY_DEPENDENCIES_SUPPORTED} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Function to check if a third-party dependency is required for a component.
#-------------------------------------------------------------------------
function(is_dependency_required_by_component component_in dependency_in is_required_out)
  set(result FALSE)
  set(depends_var "THIRD_PARTY_${dependency_in}_DEPENDENT_COMPONENTS")
  if(NOT DEFINED ${depends_var})
    message(FATAL_ERROR " is_dependency_required_by_component: ${depends_var} is not defined.
            Please add ${depends_var}
            in the 'thirdparty-dependency-definitions.cmake' file")
    return()
  endif()

  if(${component_in} IN_LIST ${depends_var})
      set(result TRUE)
      message(DEBUG "is_dependency_required_by_component: ${dependency_in} is required by component ${component_in}.")
  endif()
  set(${is_required_out} ${result} PARENT_SCOPE)
endfunction()

#-------------------------------------------------------------------------
# Check if a dependency is expected and required
#-------------------------------------------------------------------------
function (is_dependency_required dependency_in components_in is_required_out)
  if(NOT DEFINED BUILT_WITH_${dependency_in})
    message(FATAL_ERROR " is_dependency_required: The BUILT_WITH_${dependency_in} flag is required but not defined in the 'thirdparty-built-with-flags.cmake' file")
  elseif(NOT BUILT_WITH_${dependency_in})
    set(${is_required_out} FALSE PARENT_SCOPE)
  else()
    foreach(component IN LISTS ${components_in})
      set(is_required_by_component FALSE)
      is_dependency_required_by_component(${component} ${dependency} is_required_by_component)
      if(is_required_by_component)
        set(${is_required_out} TRUE PARENT_SCOPE)
        return()
      endif()
    endforeach()
  endif()
endfunction()

#-------------------------------------------------------------------------
# Check if a dependency should be found using CONFIG search mode
#-------------------------------------------------------------------------
function (is_config_mode_required dependency_in use_config_out)
  if(NOT FIND_DEPENDENCY_${dependency}_USE_CONFIG OR NOT FIND_DEPENDENCY_${dependency}_USE_CONFIG)
    set(${use_config_out} FALSE PARENT_SCOPE)
  else()
    set(${use_config_out} TRUE PARENT_SCOPE)
  endif()
endfunction()

#-------------------------------------------------------------------------
# Find all required and expected dependencies
#-------------------------------------------------------------------------
include(CMakeFindDependencyMacro)

function(find_required_dependencies components_in)
  foreach(dependency IN LISTS THIRD_PARTY_DEPENDENCIES_SUPPORTED)
    set(is_required FALSE)
    is_dependency_required(${dependency} ${components_in} is_required)
    message(DEBUG "find_required_dependencies: dependency = ${dependency}, is_required = ${is_required}")
    if(is_required)
      set(use_config FALSE)
      is_config_mode_required(${dependency} use_config)
      if(${use_config})
        message(DEBUG "find_required_dependencies: calling find_dependency(${dependency} CONFIG)...")
        find_dependency(${dependency} CONFIG)
      else()
        message(DEBUG "find_required_dependencies: calling find_dependency(${dependency})...")
        find_dependency(${dependency})
      endif()
      if(${dependency}_FOUND AND DEFINED BUILT_WITH_${dependency}_VERSION AND DEFINED ${dependency}_VERSION)
        if(NOT ${dependency}_VERSION VERSION_EQUAL ${BUILT_WITH_${dependency}_VERSION})
          message(WARNING "find_required_dependencies: found ${dependency} version ${${dependency}_VERSION} which does not match the opentelemetry-cpp built with version ${BUILT_WITH_${dependency}_VERSION}.")
        endif()
      endif()
    endif()
  endforeach()
endfunction()