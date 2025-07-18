# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_OTLP_HTTP)
  add_subdirectory(otlp)
endif()

if(WITH_CONFIGURATION)
  add_subdirectory(configuration)
endif()
