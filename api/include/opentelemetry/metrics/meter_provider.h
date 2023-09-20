// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
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

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  /**
   * Gets or creates a named Meter instance.
   *
   * @since ABI_VERSION 2
   *
   * @param[in] library_name Meter instrumentation scope
   * @param[in] library_version Version, optional
   * @param[in] schema_url Schema URL, optional
   * @param[in] attributes Instrumentation scope attributes, optional
   */
  virtual nostd::shared_ptr<Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version         = "",
      nostd::string_view schema_url              = "",
      const common::KeyValueIterable *attributes = nullptr) noexcept = 0;
#else
  /**
   * Gets or creates a named Meter instance.
   *
   * @since ABI_VERSION 1
   *
   * @param[in] library_name Meter instrumentation scope
   * @param[in] library_version Version, optional
   * @param[in] schema_url Schema URL, optional
   */
  virtual nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                            nostd::string_view library_version = "",
                                            nostd::string_view schema_url      = "") noexcept = 0;
#endif

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  nostd::shared_ptr<Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version,
      nostd::string_view schema_url,
      std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes)
  {
    nostd::span<const std::pair<nostd::string_view, common::AttributeValue>> atributes_span{
        attributes.begin(), attributes.end()};

    common::KeyValueIterableView<
        nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>>
        iterable_attributes{atributes_span};

    return GetMeter(library_name, library_version, schema_url, &iterable_attributes);
  }
#endif

#ifdef ENABLE_REMOVE_METER_PREVIEW
  virtual void RemoveMeter(nostd::string_view library_name,
                           nostd::string_view library_version = "",
                           nostd::string_view schema_url      = "") noexcept = 0;
#endif
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
