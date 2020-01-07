#pragma once

#include <map>
#include <memory>
#include <string>

#include "./tracer.h"

namespace opentelemetry
{
namespace trace
{
class Tracer;
class TracerRegistry
{
 public:
  static TracerRegistry* getInstance();
  Tracer* const get(const std::string&);
  Tracer* const get(const std::string&, const std::string&);

 private:
  TracerRegistry();
  static TracerRegistry* instance;
  std::map<const std::string, std::unique_ptr<Tracer>> tracers;
};
}  // namespace trace
}  // namespace opentelemetry
