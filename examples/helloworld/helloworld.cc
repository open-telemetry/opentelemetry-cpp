// Copyright 2019, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <opentelemetry/trace/span_id.h>

#include <cstdint>
#include <iostream>

int main()
{
  constexpr uint8_t id[] = {1, 2, 3, 4, 0x50, 0x60, 0x70, 0x80};
  opentelemetry::trace::SpanId span_id(id);
  std::cout << "Span ID: '" << span_id.ToHex() << "'\n";
}
