# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# This script enables ABI V2 with all options except preview features

set(WITH_ABI_VERSION_1 OFF CACHE BOOL "" FORCE)
set(WITH_ABI_VERSION_2 ON CACHE BOOL "" FORCE)

set(ENABLE_PREVIEW OFF)
include(${CMAKE_CURRENT_LIST_DIR}/all-options.cmake)
