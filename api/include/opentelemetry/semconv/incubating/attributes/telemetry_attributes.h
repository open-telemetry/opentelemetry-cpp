

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
 * The name of the auto instrumentation agent or distribution, if used.
 * Note: Official auto instrumentation agents and distributions SHOULD set the @code
 * telemetry.distro.name @endcode attribute to a string starting with @code opentelemetry- @endcode,
 * e.g. @code opentelemetry-java-instrumentation @endcode.
 */
static const char *kTelemetryDistroName = "telemetry.distro.name";

/**
 * The version string of the auto instrumentation agent or distribution, if used.
 */
static const char *kTelemetryDistroVersion = "telemetry.distro.version";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes. @endcode.
 */
static const char *kTelemetrySdkLanguage = "telemetry.sdk.language";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes. @endcode.
 */
static const char *kTelemetrySdkName = "telemetry.sdk.name";

/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes. @endcode.
 */
static const char *kTelemetrySdkVersion = "telemetry.sdk.version";

// @deprecated(reason="Deprecated in favor of stable
// `opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues`.")  # type:
// ignore DEBUG: {"brief": "The language of the telemetry SDK.\n", "name": "telemetry.sdk.language",
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
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kCpp @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "cpp", "note": none, "stability": "stable",
// "value": "cpp"}
static constexpr const char *kCpp = "cpp";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kDotnet
 * @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "dotnet", "note": none, "stability": "stable",
// "value": "dotnet"}
static constexpr const char *kDotnet = "dotnet";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kErlang
 * @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "erlang", "note": none, "stability": "stable",
// "value": "erlang"}
static constexpr const char *kErlang = "erlang";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kGo @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "go", "note": none, "stability": "stable",
// "value": "go"}
static constexpr const char *kGo = "go";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kJava @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "java", "note": none, "stability": "stable",
// "value": "java"}
static constexpr const char *kJava = "java";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kNodejs
 * @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "nodejs", "note": none, "stability": "stable",
// "value": "nodejs"}
static constexpr const char *kNodejs = "nodejs";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kPhp @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "php", "note": none, "stability": "stable",
// "value": "php"}
static constexpr const char *kPhp = "php";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kPython
 * @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "python", "note": none, "stability": "stable",
// "value": "python"}
static constexpr const char *kPython = "python";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kRuby @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "ruby", "note": none, "stability": "stable",
// "value": "ruby"}
static constexpr const char *kRuby = "ruby";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kRust @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "rust", "note": none, "stability": "stable",
// "value": "rust"}
static constexpr const char *kRust = "rust";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kSwift @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "swift", "note": none, "stability": "stable",
// "value": "swift"}
static constexpr const char *kSwift = "swift";
/**
 * @Deprecated in favor of stable :py:const:@code
 * opentelemetry.semconv.attributes.telemetry_attributes.TelemetrySdkLanguageValues.kWebjs @endcode.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "webjs", "note": none, "stability": "stable",
// "value": "webjs"}
static constexpr const char *kWebjs = "webjs";
}  // namespace TelemetrySdkLanguageValues

}  // namespace telemetry
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
