

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
namespace log
{

/**
 * The basename of the file.
 */
static const char *kLogFileName = "log.file.name";

/**
 * The basename of the file, with symlinks resolved.
 */
static const char *kLogFileNameResolved = "log.file.name_resolved";

/**
 * The full path to the file.
 */
static const char *kLogFilePath = "log.file.path";

/**
 * The full path to the file, with symlinks resolved.
 */
static const char *kLogFilePathResolved = "log.file.path_resolved";

/**
 * The stream associated with the log. See below for a list of well-known values.
 */
static const char *kLogIostream = "log.iostream";

/**
 * The complete orignal Log Record.
 * Note: This value MAY be added when processing a Log Record which was originally transmitted as a
 * string or equivalent data type AND the Body field of the Log Record does not contain the same
 * value. (e.g. a syslog or a log record read from a file.).
 */
static const char *kLogRecordOriginal = "log.record.original";

/**
 * A unique identifier for the Log Record.
 * Note: If an id is provided, other log records with the same id will be considered duplicates and
 * can be removed safely. This means, that two distinguishable log records MUST have different
 * values. The id MAY be an <a href="https://github.com/ulid/spec">Universally Unique
 * Lexicographically Sortable Identifier (ULID)</a>, but other identifiers (e.g. UUID) may be used
 * as needed.
 */
static const char *kLogRecordUid = "log.record.uid";

// DEBUG: {"brief": "The stream associated with the log. See below for a list of well-known
// values.\n", "name": "log.iostream", "requirement_level": "recommended", "root_namespace": "log",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "Logs
// from stdout stream", "deprecated": none, "id": "stdout", "note": none, "stability":
// "experimental", "value": "stdout"}, {"brief": "Events from stderr stream", "deprecated": none,
// "id": "stderr", "note": none, "stability": "experimental", "value": "stderr"}]}}
namespace LogIostreamValues
{
/**
 * Logs from stdout stream.
 */
// DEBUG: {"brief": "Logs from stdout stream", "deprecated": none, "id": "stdout", "note": none,
// "stability": "experimental", "value": "stdout"}
static constexpr const char *kStdout = "stdout";
/**
 * Events from stderr stream.
 */
// DEBUG: {"brief": "Events from stderr stream", "deprecated": none, "id": "stderr", "note": none,
// "stability": "experimental", "value": "stderr"}
static constexpr const char *kStderr = "stderr";
}  // namespace LogIostreamValues

}  // namespace log
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
