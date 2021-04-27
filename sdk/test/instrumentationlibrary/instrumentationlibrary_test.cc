// Copyright 2021, OpenTelemetry Authors
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

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationlibrary;

TEST(InstrumentationLibrary, CreateInstrumentationLibrary)
{

  std::string library_name     = "opentelemetry-cpp";
  std::string library_version  = "0.1.0";
  auto instrumentation_library = InstrumentationLibrary::create(library_name, library_version);

  EXPECT_EQ(instrumentation_library->GetName(), library_name);
  EXPECT_EQ(instrumentation_library->GetVersion(), library_version);
}
