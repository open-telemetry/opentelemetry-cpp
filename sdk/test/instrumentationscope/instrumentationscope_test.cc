// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace opentelemetry;
using namespace opentelemetry::sdk::instrumentationscope;

TEST(InstrumentationScope, CreateInstrumentationScope)
{

  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  auto instrumentation_scope =
      InstrumentationScope::Create(library_name, library_version, schema_url);

  EXPECT_EQ(instrumentation_scope->GetName(), library_name);
  EXPECT_EQ(instrumentation_scope->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_scope->GetSchemaURL(), schema_url);
}

TEST(InstrumentationScope, LegacyInstrumentationLibrary)
{

  std::string library_name    = "opentelemetry-cpp";
  std::string library_version = "0.1.0";
  std::string schema_url      = "https://opentelemetry.io/schemas/1.2.0";
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create(
          library_name, library_version, schema_url);

  EXPECT_EQ(instrumentation_library->GetName(), library_name);
  EXPECT_EQ(instrumentation_library->GetVersion(), library_version);
  EXPECT_EQ(instrumentation_library->GetSchemaURL(), schema_url);
}
