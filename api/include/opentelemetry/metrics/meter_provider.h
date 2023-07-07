// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
class KeyValueIterable;
};

namespace metrics
{

class Meter;

/**
 * Creates new Meter instances.
 */
class MeterProvider
{
public:
  virtual ~MeterProvider() = default;
  /**
   * Gets or creates a named Meter instance.
   *
   * Optionally a version can be passed to create a named and versioned Meter
   * instance.
   */

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  virtual nostd::shared_ptr<Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version         = "",
      nostd::string_view schema_url              = "",
      const common::KeyValueIterable *attributes = nullptr) noexcept = 0;
#else
  virtual nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                            nostd::string_view library_version = "",
                                            nostd::string_view schema_url      = "") noexcept = 0;
#endif
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
