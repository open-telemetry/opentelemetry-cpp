// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stddef.h>

#include <opentelemetry/version.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace utf8_range
{

// Returns 1 if the sequence of characters is a valid UTF-8 sequence, otherwise
// 0.
int utf8_range_IsValid(const char *data, size_t len);

// Returns the length in bytes of the prefix of str that is all
// structurally valid UTF-8.
size_t utf8_range_ValidPrefix(const char *data, size_t len);

}  // namespace utf8_range
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
