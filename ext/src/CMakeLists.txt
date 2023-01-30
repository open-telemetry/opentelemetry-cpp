# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_ZPAGES)
  add_subdirectory(zpages)
endif()

if(WITH_HTTP_CLIENT_CURL)
  add_subdirectory(http/client/curl)
endif()
