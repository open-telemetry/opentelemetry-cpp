#pragma once

#include "opentelemetry/plugin/tracer_handle.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
namespace api = opentelemetry::trace;

class TracerHandle final : public plugin::TracerHandle
{
public:
  explicit TracerHandle(std::shared_ptr<api::Tracer> tracer) noexcept : tracer_{std::move(tracer)}
  {}

  // opentelemetry::plugin::TracerHandle
  api::Tracer &tracer() const noexcept override { return tracer_; }

private:
  std::shared_ptr<api::Tracer> tracer_;
};
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
