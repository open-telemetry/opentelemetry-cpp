# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Release-only x64-windows (dynamic libraries, dynamic CRT)
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_BUILD_TYPE release)
