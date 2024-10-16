

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
namespace cpu
{

/**
 * The mode of the CPU.
 */
static const char *kCpuMode = "cpu.mode";

// DEBUG: {"brief": "The mode of the CPU", "examples": ["user", "system"], "name": "cpu.mode",
// "requirement_level": "recommended", "root_namespace": "cpu", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "user",
// "note": none, "stability": "experimental", "value": "user"}, {"brief": none, "deprecated": none,
// "id": "system", "note": none, "stability": "experimental", "value": "system"}, {"brief": none,
// "deprecated": none, "id": "nice", "note": none, "stability": "experimental", "value": "nice"},
// {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability": "experimental",
// "value": "idle"}, {"brief": none, "deprecated": none, "id": "iowait", "note": none, "stability":
// "experimental", "value": "iowait"}, {"brief": none, "deprecated": none, "id": "interrupt",
// "note": none, "stability": "experimental", "value": "interrupt"}, {"brief": none, "deprecated":
// none, "id": "steal", "note": none, "stability": "experimental", "value": "steal"}, {"brief":
// none, "deprecated": none, "id": "kernel", "note": none, "stability": "experimental", "value":
// "kernel"}]}}
namespace CpuModeValues
{
/**
 * user.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "user", "note": none, "stability":
// "experimental", "value": "user"}
static constexpr const char *kUser = "user";
/**
 * system.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "system", "note": none, "stability":
// "experimental", "value": "system"}
static constexpr const char *kSystem = "system";
/**
 * nice.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "nice", "note": none, "stability":
// "experimental", "value": "nice"}
static constexpr const char *kNice = "nice";
/**
 * idle.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
/**
 * iowait.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "iowait", "note": none, "stability":
// "experimental", "value": "iowait"}
static constexpr const char *kIowait = "iowait";
/**
 * interrupt.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "interrupt", "note": none, "stability":
// "experimental", "value": "interrupt"}
static constexpr const char *kInterrupt = "interrupt";
/**
 * steal.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "steal", "note": none, "stability":
// "experimental", "value": "steal"}
static constexpr const char *kSteal = "steal";
/**
 * kernel.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "kernel", "note": none, "stability":
// "experimental", "value": "kernel"}
static constexpr const char *kKernel = "kernel";
}  // namespace CpuModeValues

}  // namespace cpu
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
