# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_ZPAGES)
  add_subdirectory(zpages)
endif()
add_subdirectory(http)
if(BUILD_W3CTRACECONTEXT_TEST)
  add_subdirectory(w3c_tracecontext_test)
endif()
