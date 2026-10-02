# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# All examples options

if(NOT DEFINED ENABLE_EXAMPLES)
    set(ENABLE_EXAMPLES ON)
endif()

set(OTELCPP_WITH_EXAMPLES ${ENABLE_EXAMPLES} CACHE BOOL "" FORCE)
set(OTELCPP_WITH_EXAMPLES_HTTP ${ENABLE_EXAMPLES} CACHE BOOL "" FORCE)
