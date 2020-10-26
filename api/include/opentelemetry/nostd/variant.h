// Copyright 2020, OpenTelemetry Authors
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

#pragma once

#ifdef HAVE_CPP_STDLIB
#  include "opentelemetry/std/variant.h"
#elif defined(HAVE_ABSEIL)
// TODO: prefer `absl::variant` over `nostd::variant` since the latter does not compile with vs2015
#  include "absl/types/variant.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
using absl::get;
using absl::holds_alternative;
using absl::variant;
using absl::visit;
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
#else
#  include "opentelemetry/nostd/mpark/variant.h"
#endif
