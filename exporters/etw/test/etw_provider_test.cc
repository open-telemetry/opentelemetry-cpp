// Copyright 2020, OpenTelemetry Authors
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

#ifdef _WIN32

#  include <gtest/gtest.h>
#  include <string>

/* TODO: this definition needs to be removed when TraceLoggingDynamic.h is OSS */
#  ifndef HAVE_NO_TLD
#    define HAVE_NO_TLD
#  endif

#  include "opentelemetry/exporters/etw/etw_provider_exporter.h"

using namespace OPENTELEMETRY_NAMESPACE;

TEST(ETWProvider, ProviderIsRegisteredSuccessfully)
{
  std::string providerName = "OpenTelemetry";
  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str());

  bool registered = etw.is_registered(providerName);
  ASSERT_TRUE(registered);
}

TEST(ETWProvider, ProviderIsNotRegisteredSuccessfully)
{
  std::string providerName = "Telemetry";
  static ETWProvider etw;

  bool registered = etw.is_registered(providerName);
  ASSERT_FALSE(registered);
}

TEST(ETWProvider, CheckOpenGUIDDataSuccessfully)
{
  std::string providerName = "OpenTelemetry";

  // get GUID from the handle returned
  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str());

  utils::UUID uuid_handle(handle.providerGuid);
  auto guidStrHandle = uuid_handle.to_string();

  // get GUID from the providerName
  auto guid = utils::GetProviderGuid(providerName.c_str());
  utils::UUID uuid_name(guid);
  auto guidStrName = uuid_name.to_string();

  ASSERT_STREQ(guidStrHandle.c_str(), guidStrName.c_str());
}

TEST(ETWProvider, CheckCloseSuccess)
{
  std::string providerName = "OpenTelemetry";

  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str());

  auto result = etw.close(handle);
  ASSERT_NE(result, etw.STATUS_ERROR);
}

#endif
