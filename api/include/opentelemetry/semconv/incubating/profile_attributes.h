/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_attributes-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace profile
{

/**
 * Describes the interpreter or compiler of a single frame.
 */
static constexpr const char *kProfileFrameType = "profile.frame.type";

namespace ProfileFrameTypeValues
{
/**
 * <a href="https://wikipedia.org/wiki/.NET">.NET</a>
 */
static constexpr const char *kDotnet = "dotnet";

/**
 * <a href="https://wikipedia.org/wiki/Java_virtual_machine">JVM</a>
 */
static constexpr const char *kJvm = "jvm";

/**
 * <a href="https://wikipedia.org/wiki/Kernel_(operating_system)">Kernel</a>
 */
static constexpr const char *kKernel = "kernel";

/**
 * <a href="https://wikipedia.org/wiki/C_(programming_language)">C</a>, <a
 * href="https://wikipedia.org/wiki/C%2B%2B">C++</a>, <a
 * href="https://wikipedia.org/wiki/Go_(programming_language)">Go</a>, <a
 * href="https://wikipedia.org/wiki/Rust_(programming_language)">Rust</a>
 */
static constexpr const char *kNative = "native";

/**
 * <a href="https://wikipedia.org/wiki/Perl">Perl</a>
 */
static constexpr const char *kPerl = "perl";

/**
 * <a href="https://wikipedia.org/wiki/PHP">PHP</a>
 */
static constexpr const char *kPhp = "php";

/**
 * <a href="https://wikipedia.org/wiki/Python_(programming_language)">Python</a>
 */
static constexpr const char *kCpython = "cpython";

/**
 * <a href="https://wikipedia.org/wiki/Ruby_(programming_language)">Ruby</a>
 */
static constexpr const char *kRuby = "ruby";

/**
 * <a href="https://wikipedia.org/wiki/V8_(JavaScript_engine)">V8JS</a>
 */
static constexpr const char *kV8js = "v8js";

}  // namespace ProfileFrameTypeValues

}  // namespace profile
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
