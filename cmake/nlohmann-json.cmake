if("${nlohmann-json}" STREQUAL "")
    set(nlohmann-json "main")
endif()
include(ExternalProject)
ExternalProject_Add(nlohmann_json_download
    PREFIX 3rd_party
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG
        "${nlohmann-json}"
    UPDATE_COMMAND ""
    CMAKE_ARGS
      -DJSON_BuildTests=OFF
      -DJSON_Install=ON
    TEST_AFTER_INSTALL
        0
    DOWNLOAD_NO_PROGRESS
        1
    LOG_CONFIGURE
        1
    LOG_BUILD
        1
    LOG_INSTALL
        1
)

SET(NLOHMANN_JSON_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rd_party/src/nlohmann_json_download/single_include)
SET(NLOHMANN_JSON_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rd_party/src/nlohmann_json_download/lib)

add_library(nlohmann_json_ INTERFACE)
target_include_directories(nlohmann_json_ INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
add_dependencies(nlohmann_json_ nlohmann_json_download)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json_)
