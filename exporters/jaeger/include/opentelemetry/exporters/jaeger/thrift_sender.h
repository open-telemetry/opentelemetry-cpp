// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <agent.h>
#include <opentelemetry/version.h>
#include <memory>
#include <vector>

#include "recordable.h"
#include "transport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

using namespace jaegertracing;

class ThriftSender
{
public:
  ThriftSender(std::unique_ptr<Transport> &&transport);
  ~ThriftSender();

  bool Append(std::unique_ptr<Recordable> &&span) noexcept;
  int Flush();

private:
  std::vector<std::unique_ptr<Recordable>> spans_;
  std::unique_ptr<Transport> transport_;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
