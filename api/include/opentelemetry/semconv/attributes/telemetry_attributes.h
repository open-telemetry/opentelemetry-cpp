

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
namespace telemetry
{

/**
 * The language of the telemetry SDK.
 */
static const char *kTelemetrySdkLanguage = "telemetry.sdk.language";

/**
 * The name of the telemetry SDK as defined above.
 * Note: The OpenTelemetry SDK MUST set the @code telemetry.sdk.name @endcode attribute to @code
 * opentelemetry @endcode. If another SDK, like a fork or a vendor-provided implementation, is used,
 * this SDK MUST set the
 * @code telemetry.sdk.name @endcode attribute to the fully-qualified class or module name of this
 * SDK's main entry point or another suitable identifier depending on the language. The identifier
 * @code opentelemetry @endcode is reserved and MUST NOT be used in this case. All custom
 * identifiers SHOULD be stable across different versions of an implementation.
 */
static const char *kTelemetrySdkName = "telemetry.sdk.name";

/**
 * The version string of the telemetry SDK.
 */
static const char *kTelemetrySdkVersion = "telemetry.sdk.version";

// DEBUG: {"brief": "The language of the telemetry SDK.\n", "name": "telemetry.sdk.language",
// "requirement_level": "required", "root_namespace": "telemetry", "stability": "stable", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "cpp",
// "note": none, "stability": "stable", "value": "cpp"}, {"brief": none, "deprecated": none, "id":
// "dotnet", "note": none, "stability": "stable", "value": "dotnet"}, {"brief": none, "deprecated":
// none, "id": "erlang", "note": none, "stability": "stable", "value": "erlang"}, {"brief": none,
// "deprecated": none, "id": "go", "note": none, "stability": "stable", "value": "go"}, {"brief":
// none, "deprecated": none, "id": "java", "note": none, "stability": "stable", "value": "java"},
// {"brief": none, "deprecated": none, "id": "nodejs", "note": none, "stability": "stable", "value":
// "nodejs"}, {"brief": none, "deprecated": none, "id": "php", "note": none, "stability": "stable",
// "value": "php"}, {"brief": none, "deprecated": none, "id": "python", "note": none, "stability":
// "stable", "value": "python"}, {"brief": none, "deprecated": none, "id": "ruby", "note": none,
// "stability": "stable", "value": "ruby"}, {"brief": none, "deprecated": none, "id": "rust",
// "note": none, "stability": "stable", "value": "rust"}, {"brief": none, "deprecated": none, "id":
// "swift", "note": none, "stability": "stable", "value": "swift"}, {"brief": none, "deprecated":
// none, "id": "webjs", "note": none, "stability": "stable", "value": "webjs"}]}}
namespace TelemetrySdkLanguageValues
{
/**
 * cpp.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "cpp", "note": none, "stability": "stable",
// "value": "cpp"}
static constexpr const char *kCpp = "cpp";
/**
 * dotnet.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "dotnet", "note": none, "stability": "stable",
// "value": "dotnet"}
static constexpr const char *kDotnet = "dotnet";
/**
 * erlang.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "erlang", "note": none, "stability": "stable",
// "value": "erlang"}
static constexpr const char *kErlang = "erlang";
/**
 * go.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "go", "note": none, "stability": "stable",
// "value": "go"}
static constexpr const char *kGo = "go";
/**
 * java.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "java", "note": none, "stability": "stable",
// "value": "java"}
static constexpr const char *kJava = "java";
/**
 * nodejs.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "nodejs", "note": none, "stability": "stable",
// "value": "nodejs"}
static constexpr const char *kNodejs = "nodejs";
/**
 * php.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "php", "note": none, "stability": "stable",
// "value": "php"}
static constexpr const char *kPhp = "php";
/**
 * python.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "python", "note": none, "stability": "stable",
// "value": "python"}
static constexpr const char *kPython = "python";
/**
 * ruby.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ruby", "note": none, "stability": "stable",
// "value": "ruby"}
static constexpr const char *kRuby = "ruby";
/**
 * rust.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "rust", "note": none, "stability": "stable",
// "value": "rust"}
static constexpr const char *kRust = "rust";
/**
 * swift.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "swift", "note": none, "stability": "stable",
// "value": "swift"}
static constexpr const char *kSwift = "swift";
/**
 * webjs.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "webjs", "note": none, "stability": "stable",
// "value": "webjs"}
static constexpr const char *kWebjs = "webjs";
}  // namespace TelemetrySdkLanguageValues

}  // namespace telemetry
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
