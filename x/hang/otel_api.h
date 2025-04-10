#pragma once

namespace otel_api
{
// Quickly check if otel_sdk_{r,d,rd} has been loaded, and ready to use.
bool enabled() noexcept;

} // namespace otel_api