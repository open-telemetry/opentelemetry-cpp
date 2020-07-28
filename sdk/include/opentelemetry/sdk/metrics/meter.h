#pragma once

#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Meter final : public opentelemetry::metrics::Meter, public std::enable_shared_from_this<Meter>
{
public:

};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
