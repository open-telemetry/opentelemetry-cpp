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
namespace event
{

/**
 * Identifies the class / type of event.
 * <p>
 * Event names are subject to the same rules as <a
 * href="/docs/general/attribute-naming.md">attribute names</a>. Notably, event names are namespaced
 * to avoid collisions and provide a clean separation of semantics for events in separate domains
 * like browser, mobile, and kubernetes.
 */
static constexpr const char *kEventName = "event.name";

}  // namespace event
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
