macro(check_append_cxx_compiler_flag OUTPUT_VAR)
  foreach(CHECK_FLAG ${ARGN})
    check_cxx_compiler_flag(${CHECK_FLAG}
                            "check_cxx_compiler_flag_${CHECK_FLAG}")
    if(check_cxx_compiler_flag_${CHECK_FLAG})
      list(APPEND ${OUTPUT_VAR} ${CHECK_FLAG})
    endif()
  endforeach()
endmacro()

if(NOT PATCH_PROTOBUF_SOURCES_OPTIONS_SET)
  if(MSVC)
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS /wd4244 /wd4251 /wd4267 /wd4309)

    if(MSVC_VERSION GREATER_EQUAL 1922)
      # see
      # https://docs.microsoft.com/en-us/cpp/overview/cpp-conformance-improvements?view=vs-2019#improvements_162
      # for detail
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd5054)
    endif()

    if(MSVC_VERSION GREATER_EQUAL 1925)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4996)
    endif()

    if(MSVC_VERSION LESS 1910)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4800)
    endif()
  else()
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    include(CheckCXXCompilerFlag)
    check_append_cxx_compiler_flag(
      PATCH_PROTOBUF_SOURCES_OPTIONS -Wno-type-limits
      -Wno-deprecated-declarations -Wno-unused-parameter)
  endif()
  set(PATCH_PROTOBUF_SOURCES_OPTIONS_SET TRUE)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS
        ${PATCH_PROTOBUF_SOURCES_OPTIONS}
        CACHE INTERNAL
              "Options to disable warning of generated protobuf sources" FORCE)
  endif()
endif()

function(patch_protobuf_sources)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_SRC ${ARGN})
      unset(PROTO_SRC_OPTIONS)
      get_source_file_property(PROTO_SRC_OPTIONS ${PROTO_SRC} COMPILE_OPTIONS)
      if(PROTO_SRC_OPTIONS)
        list(APPEND PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_source_files_properties(
        ${PROTO_SRC} PROPERTIES COMPILE_OPTIONS "${PROTO_SRC_OPTIONS}")
    endforeach()
    unset(PROTO_SRC)
    unset(PROTO_SRC_OPTIONS)
  endif()
endfunction()

function(patch_protobuf_targets)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_TARGET ${ARGN})
      unset(PROTO_TARGET_OPTIONS)
      get_target_property(PROTO_TARGET_OPTIONS ${PROTO_TARGET} COMPILE_OPTIONS)
      if(PROTO_TARGET_OPTIONS)
        list(APPEND PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_target_properties(
        ${PROTO_TARGET} PROPERTIES COMPILE_OPTIONS "${PROTO_TARGET_OPTIONS}")
    endforeach()
    unset(PROTO_TARGET)
    unset(PROTO_TARGET_OPTIONS)
  endif()
endfunction()
