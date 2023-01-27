// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
/**
 * Creates new Tracer instances.
 */
class OPENTELEMETRY_EXPORT TracerProvider
{
public:
  virtual ~TracerProvider() = default;
  /**
   * Gets or creates a named tracer instance.
   *
   * Optionally a version can be passed to create a named and versioned tracer
   * instance.
   */
  virtual nostd::shared_ptr<Tracer> GetTracer(nostd::string_view library_name,
                                              nostd::string_view library_version = "",
                                              nostd::string_view schema_url      = "") noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE

#if defined(OPENTELEMETRY_EXPORT)

namespace std
{

//
// Partial specialization of default_delete used by unique_ptr or shared_ptr.
// This makes the delete of the type in unique_ptr/shared_ptr happening in the
// DLL in which it is allocated.
//
template <>
struct OPENTELEMETRY_EXPORT default_delete<OPENTELEMETRY_NAMESPACE::trace::TracerProvider>
{
public:
  void operator()(OPENTELEMETRY_NAMESPACE::trace::TracerProvider *tracer_provider)
  {
    delete tracer_provider;
  }
};

}  // namespace std

#endif  // OPENTELEMETRY_EXPORT
