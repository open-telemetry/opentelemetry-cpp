// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// This file is intentionally kept empty for ABI compatibility.
// BatchSpanProcessorOptions is now an aggregate type with default member initializers
// defined in the header file. The environment variable reading logic has been moved
// to inline helper functions in the header to restore C++20 aggregate initialization
// support (designated initializers).
