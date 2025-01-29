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
namespace code
{

/**
 * Deprecated, use @code code.column.number @endcode
 * <p>
 * @deprecated
 * Replaced by @code code.column.number @endcode
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kCodeColumn = "code.column";

/**
 * The column number in @code code.file.path @endcode best representing the operation. It SHOULD
 * point within the code unit named in @code code.function.name @endcode.
 */
static constexpr const char *kCodeColumnNumber = "code.column.number";

/**
 * The source code file name that identifies the code unit as uniquely as possible (preferably an
 * absolute file path).
 */
static constexpr const char *kCodeFilePath = "code.file.path";

/**
 * Deprecated, use @code code.file.path @endcode instead
 */
static constexpr const char *kCodeFilepath = "code.filepath";

/**
 * Deprecated, use @code code.function.name @endcode instead
 * <p>
 * @deprecated
 * Replaced by @code code.function.name @endcode
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kCodeFunction = "code.function";

/**
 * The method or function name, or equivalent (usually rightmost part of the code unit's name).
 */
static constexpr const char *kCodeFunctionName = "code.function.name";

/**
 * The line number in @code code.file.path @endcode best representing the operation. It SHOULD point
 * within the code unit named in @code code.function.name @endcode.
 */
static constexpr const char *kCodeLineNumber = "code.line.number";

/**
 * Deprecated, use @code code.line.number @endcode instead
 * <p>
 * @deprecated
 * Replaced by @code code.line.number @endcode
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kCodeLineno = "code.lineno";

/**
 * The "namespace" within which @code code.function.name @endcode is defined. Usually the qualified
 * class or module name, such that @code code.namespace @endcode + some separator + @code
 * code.function.name @endcode form a unique identifier for the code unit.
 */
static constexpr const char *kCodeNamespace = "code.namespace";

/**
 * A stacktrace as a string in the natural representation for the language runtime. The
 * representation is to be determined and documented by each language SIG.
 */
static constexpr const char *kCodeStacktrace = "code.stacktrace";

}  // namespace code
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
