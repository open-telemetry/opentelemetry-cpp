#pragma once

#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/version.h"

#include <memory>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Meter final : public opentelemetry::metrics::Meter, public std::enable_shared_from_this<Meter>
{
public:
  explicit Meter(std::string library_name, std::string library_version)
  {
    library_name_    = library_name;
    library_version_ = library_version;
  }

private:
  std::string library_name_;
  std::string library_version_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
