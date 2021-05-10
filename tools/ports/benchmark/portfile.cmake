if(VCPKG_CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
  message(FATAL_ERROR "${PORT} does not currently support UWP")
endif()

if (VCPKG_PLATFORM_TOOLSET STREQUAL "v140")
  # set(CMAKE_C_COMPILER_WORKS 1)
  # set(CMAKE_CXX_COMPILER_WORKS 1)
  set(CMAKE_C_COMPILER cl.exe)
  set(CMAKE_CXX_COMPILER cl.exe)
  set(MSVC_TOOLSET_VERSION 140)
  # set(VCPKG_VISUAL_STUDIO_PATH "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0")
  # set(VCPKG_PLATFORM_TOOLSET v140)
else()
  # Make sure vs2019 compiled binaries are compat with vs2017
  set(VCPKG_CXX_FLAGS "/Zc:__cplusplus /d2FH4-")
  set(VCPKG_C_FLAGS "/Zc:__cplusplus /d2FH4-")
  set(PREFER PREFER_NINJA)
endif()

include(vcpkg_common_functions)

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO google/benchmark
    HEAD_REF master
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    ${PREFER}
    OPTIONS
        -DBENCHMARK_ENABLE_TESTING=OFF
        -DCMAKE_DEBUG_POSTFIX=d
)

vcpkg_install_cmake()

vcpkg_copy_pdbs()

vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/benchmark)

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)

# Handle copyright
file(COPY ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/benchmark)
file(RENAME ${CURRENT_PACKAGES_DIR}/share/benchmark/LICENSE ${CURRENT_PACKAGES_DIR}/share/benchmark/copyright)
