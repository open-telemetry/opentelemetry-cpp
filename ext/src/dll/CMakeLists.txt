# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set(OPENTELEMETRY_EXPORT_DEF
    "${CMAKE_CURRENT_BINARY_DIR}/opentelemetry_cpp.def")

add_library(opentelemetry_cpp SHARED dllmain.cc ${OPENTELEMETRY_EXPORT_DEF})

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_trace
                            opentelemetry_exporter_ostream_span)

if(WITH_OTLP_GRPC)
  add_compile_definitions(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc)
endif()

if(WITH_OTLP_HTTP)
  add_compile_definitions(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http)
endif()

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_metrics
                            opentelemetry_exporter_ostream_metrics)

if(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc_metrics)
endif()

if(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http_metric)
endif()

target_link_libraries(
  opentelemetry_cpp PRIVATE opentelemetry_logs
                            opentelemetry_exporter_ostream_logs)

if(WITH_OTLP_GRPC)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_grpc_log)
endif()

if(WITH_OTLP_HTTP)
  target_link_libraries(opentelemetry_cpp
                        PRIVATE opentelemetry_exporter_otlp_http_log)
endif()

find_program(
  _vswhere_tool
  NAMES vswhere
  PATHS "$ENV{ProgramFiles\(x86\)}/Microsoft Visual Studio/Installer")
if(NOT ${vswhere})
  message(
    FATAL_ERROR "Could not locate vswhere.exe - unable to locate dumpbin.exe")
endif()

execute_process(
  COMMAND "${_vswhere_tool}" -latest -find **/dumpbin.exe
  OUTPUT_VARIABLE DUMPBIN_PATHS
  RESULT_VARIABLE _vs_where_exitcode
  OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "DUMPBIN PATHS: ${DUMPBIN_PATHS} ${RESULT_VARIABLE}")

string(REPLACE "\n" ";" DUMPBIN_PATHS_LIST ${DUMPBIN_PATHS})

list(POP_FRONT DUMPBIN_PATHS_LIST DUMPBIN_PATH)

message(STATUS "DUMPBIN PATH: ${DUMPBIN_PATH}")

# find Powershell executable find_program(DUMPBIN_PATH NAMES dumpbin REQUIRED)

# find Powershell executable
find_program(POWERSHELL_PATH NAMES pwsh powershell REQUIRED)

set(OPENTELEMETRY_INPUT_DEF "${CMAKE_CURRENT_SOURCE_DIR}/input.txt")

add_custom_command(
  OUTPUT ${OPENTELEMETRY_INPUT_DEF}
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/input.src"
  COMMAND
    ${CMAKE_CXX_COMPILER}
    "-D$<JOIN:$<TARGET_PROPERTY:opentelemetry_cpp,COMPILE_DEFINITIONS>,;-D>" /EP
    ${CMAKE_CURRENT_SOURCE_DIR}/input.src > ${OPENTELEMETRY_INPUT_DEF}
  VERBATIM COMMAND_EXPAND_LISTS)

add_custom_command(
  OUTPUT ${OPENTELEMETRY_EXPORT_DEF}
  DEPENDS "${OPENTELEMETRY_INPUT_DEF}"
          "${CMAKE_CURRENT_SOURCE_DIR}/make_def.ps1"
  COMMAND
    ${POWERSHELL_PATH} "-ExecutionPolicy" "Bypass" "-File"
    "${CMAKE_CURRENT_SOURCE_DIR}/make_def.ps1" "-dumpbin" "${DUMPBIN_PATH}"
    "-bindir" ${PROJECT_BINARY_DIR} "-inputfile" ${OPENTELEMETRY_INPUT_DEF}
    "-targetfile" ${OPENTELEMETRY_EXPORT_DEF}
  VERBATIM)

if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS opentelemetry_cpp
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
