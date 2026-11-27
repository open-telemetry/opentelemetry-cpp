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
  The identifier of a logical execution target (CPU) as seen by the operating system, in the range
  [0..n-1]. <p> On systems with simultaneous multithreading (SMT) enabled — such as Intel
  Hyper-Threading or AMD SMT — this value will usually differ from the physical core number, because
  each core presents multiple logical CPUs for the system to use.
 */
static constexpr const char *kCpuLogicalNumber = "cpu.logical_number";

/**
  The mode of the CPU
 */
static constexpr const char *kCpuMode = "cpu.mode";

namespace CpuModeValues
{
/**
  User
 */
static constexpr const char *kUser = "user";

/**
  System
 */
static constexpr const char *kSystem = "system";

/**
  Nice
 */
static constexpr const char *kNice = "nice";

/**
  Idle
 */
static constexpr const char *kIdle = "idle";

/**
  IO Wait
 */
static constexpr const char *kIowait = "iowait";

/**
  Interrupt
 */
static constexpr const char *kInterrupt = "interrupt";

/**
  Steal
 */
static constexpr const char *kSteal = "steal";

/**
  Deprecated. Use @code system @endcode instead.

  @deprecated
  {"note": "Replaced by @code system @endcode.", "reason": "renamed", "renamed_to": "system"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kKernel = "kernel";

}  // namespace CpuModeValues

}  // namespace cpu
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
