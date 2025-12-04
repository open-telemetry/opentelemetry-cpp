# Install script for directory: /home/runner/work/opentelemetry-cpp/opentelemetry-cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/_deps/googletest-build/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/_deps/benchmark-build/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/api/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/ext/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/test_common/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/examples/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/functional/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cmake-config" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES
    "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/cmake/opentelemetry-cpp/opentelemetry-cpp-config.cmake"
    "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/cmake/opentelemetry-cpp/opentelemetry-cpp-config-version.cmake"
    "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/cmake/find-package-support-functions.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "api" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-api-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-api-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-api-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-api-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-api-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-api-target.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "api" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/api/include/opentelemetry" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/common/libopentelemetry_common.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/resource/libopentelemetry_resources.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/version/libopentelemetry_version.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/logs/libopentelemetry_logs.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/trace/libopentelemetry_trace.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/sdk/src/metrics/libopentelemetry_metrics.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-sdk-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-sdk-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-sdk-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-sdk-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-sdk-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-sdk-target.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-sdk-target-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "sdk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opentelemetry" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/sdk/include/opentelemetry/" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/sdk\\/configuration\\/[^/]*\\.h$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ext_common" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-ext_common-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-ext_common-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-ext_common-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-ext_common-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-ext_common-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-ext_common-target.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ext_common" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opentelemetry" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/ext/include/opentelemetry/ext" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_span.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_metrics.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_logs.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream-target.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream-target-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opentelemetry/exporters" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/exporters/ostream/include/opentelemetry/exporters/ostream" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/console\\_log\\_record\\_builder\\.h$" EXCLUDE REGEX "/console\\_push\\_metric\\_builder\\.h$" EXCLUDE REGEX "/console\\_span\\_builder\\.h$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream_builder" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_span_builder.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream_builder" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_metrics_builder.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream_builder" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/ostream/libopentelemetry_exporter_ostream_logs_builder.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream_builder" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream_builder-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream_builder-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream_builder-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream_builder-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_ostream_builder-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream_builder-target.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_ostream_builder-target-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_ostream_builder" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opentelemetry/exporters" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/exporters/ostream/include/opentelemetry/exporters/ostream" FILES_MATCHING REGEX "/console\\_log\\_record\\_builder\\.h$" REGEX "/console\\_push\\_metric\\_builder\\.h$" REGEX "/console\\_span\\_builder\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_in_memory" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/memory/libopentelemetry_exporter_in_memory.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_in_memory" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/exporters/memory/libopentelemetry_exporter_in_memory_metric.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_in_memory" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_in_memory-target.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_in_memory-target.cmake"
         "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_in_memory-target.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_in_memory-target-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp/opentelemetry-cpp-exporters_in_memory-target.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_in_memory-target.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/CMakeFiles/Export/f570e1c00958d1a38e3540fe920de03d/opentelemetry-cpp-exporters_in_memory-target-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "exporters_in_memory" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opentelemetry/exporters" TYPE DIRECTORY FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/exporters/memory/include/opentelemetry/exporters/memory" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cmake-config" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/cmake/component-definitions.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cmake-config" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/opentelemetry-cpp" TYPE FILE FILES "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/cmake/thirdparty-dependency-definitions.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/opentelemetry-cpp/opentelemetry-cpp/_codeql_build_dir/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
