# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

def otel_cc_library(**kwargs):
    native.cc_library(
        # Link as .o (.obj) files, not .a (.lib)
        alwayslink = True,
        # Don't create .so files for dbg/fastbuild, always create static libs
        linkstatic = True,
        **kwargs)
