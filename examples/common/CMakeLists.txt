# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(foo_library)
if(WITH_LOGS_PREVIEW)
  add_subdirectory(logs_foo_library)
endif()
add_subdirectory(metrics_foo_library)
