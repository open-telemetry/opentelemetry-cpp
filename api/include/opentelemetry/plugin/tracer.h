#pragma once

#include <memory>

#include "opentelemetry/plugin/dynamic_library_handle.h"
#include "opentelemetry/plugin/tracer_handle.h"
#include "opentelemetry/trace/tracer.h"

namespace opentelemetry
{
namespace plugin
{
class Span final : public trace::Span
{
public:
  Span(std::shared_ptr<trace::Tracer> &&tracer, std::unique_ptr<trace::Span> &&span) noexcept
      : tracer_{std::move(tracer)}, span_{std::move(span)}
  {}

  // trace::Span
  trace::Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<trace::Tracer> tracer_;
  std::unique_ptr<trace::Span> span_;
};

class Tracer final : public trace::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  Tracer(std::shared_ptr<DynamicLibraryHandle> library_handle,
         std::unique_ptr<TracerHandle> &&tracer_handle) noexcept
      : library_handle_{std::move(library_handle)}, tracer_handle_{std::move(tracer_handle)}
  {}

  // trace::Tracer
  nostd::unique_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    auto span = tracer_handle_->tracer().StartSpan(name, options);
    if (span == nullptr)
    {
      return nullptr;
    }
    return nostd::unique_ptr<trace::Span>{new (std::nothrow)
                                              Span{this->shared_from_this(), std::move(span)}};
  }

private:
  // Note: The order is important here.
  //
  // It's undefined behavior to close the library while a loaded tracer is still active.
  std::shared_ptr<DynamicLibraryHandle> library_handle_;
  std::unique_ptr<TracerHandle> tracer_handle_;
};
}  // namespace plugin
}  // namespace opentelemetry
