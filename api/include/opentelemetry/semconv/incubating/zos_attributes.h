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
namespace zos
{

/**
  The System Management Facility (SMF) identifier that uniquely identifies a z/OS system within a
  sysplex or mainframe environment and is used for system and performance analysis.

  @deprecated
  {"note": "Moved to the <a
  href="https://github.com/open-telemetry/semantic-conventions-mainframe">OpenTelemetry Mainframe
  semantic conventions repository</a>.\n", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kZosSmfId = "zos.smf.id";

/**
  The name of the sysplex to which the z/OS system belongs.

  @deprecated
  {"note": "Moved to the <a
  href="https://github.com/open-telemetry/semantic-conventions-mainframe">OpenTelemetry Mainframe
  semantic conventions repository</a>.\n", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kZosSysplexName = "zos.sysplex.name";

}  // namespace zos
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
