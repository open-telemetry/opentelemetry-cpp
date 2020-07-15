#pragma once

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/*
    Controller stub for MeterProvider
 */
class Controller
{
public:
  virtual ~Controller() = default;

};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
