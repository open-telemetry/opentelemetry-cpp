

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
namespace file
{

/**
 * Directory where the file is located. It should include the drive letter, when appropriate.
 */
static const char *kFileDirectory = "file.directory";

/**
 * File extension, excluding the leading dot.
 * Note: When the file name has multiple extensions (example.tar.gz), only the last one should be
 * captured ("gz", not "tar.gz").
 */
static const char *kFileExtension = "file.extension";

/**
 * Name of the file including the extension, without the directory.
 */
static const char *kFileName = "file.name";

/**
 * Full path to the file, including the file name. It should include the drive letter, when
 * appropriate.
 */
static const char *kFilePath = "file.path";

/**
 * File size in bytes.
 */
static const char *kFileSize = "file.size";

}  // namespace file
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
