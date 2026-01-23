// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef _WIN32

#  include <gtest/gtest.h>
#  include <string>

#  include "opentelemetry/exporters/etw/etw_provider.h"

using namespace OPENTELEMETRY_NAMESPACE;

TEST(ETWProvider, ProviderIsRegisteredSuccessfully)
{
  std::string providerName = "OpenTelemetry-ETW-Provider";
  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str());

  bool registered = etw.is_registered(providerName);
  ASSERT_TRUE(registered);
  etw.close(handle);
}

TEST(ETWProvider, ProviderIsNotRegisteredSuccessfully)
{
  std::string providerName = "OpenTelemetry-ETW-Provider-NULL";
  static ETWProvider etw;

  bool registered = etw.is_registered(providerName);
  ASSERT_FALSE(registered);
}

TEST(ETWProvider, CheckOpenGUIDDataSuccessfully)
{
  std::string providerName = "OpenTelemetry-ETW-Provider";

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
  etw.close(handle);
}

TEST(ETWProvider, CheckCloseSuccess)
{
  std::string providerName = "OpenTelemetry-ETW-Provider";

  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str(), ETWProvider::EventFormat::ETW_MANIFEST);
  auto result = etw.close(handle);
  ASSERT_NE(result, etw.STATUS_ERROR);
  ASSERT_FALSE(etw.is_registered(providerName));
}

TEST(ETWProvider, CheckCloseInfiniteLoop)
{
  std::string providerName1 = "Provider1";
  std::string providerName2 = "Provider2";

  static ETWProvider etw;
  auto handle1 = etw.open(providerName1.c_str());
  auto handle2 = etw.open(providerName2.c_str());

  // This should not hang
  auto result2 = etw.close(handle2);
  ASSERT_EQ(result2, etw.STATUS_OK);
  ASSERT_FALSE(etw.is_registered(providerName2));

  auto result1 = etw.close(handle1);
  ASSERT_EQ(result1, etw.STATUS_OK);
  ASSERT_FALSE(etw.is_registered(providerName1));
}

TEST(ETWProvider, CheckCloseRefCountUnderflow)
{
  std::string providerName = "OpenTelemetry-ETW-Provider-Underflow";
  static ETWProvider etw;
  auto handle = etw.open(providerName.c_str());
  auto result = etw.close(handle);
  ASSERT_EQ(result, etw.STATUS_OK);

  // Subsequent close should fail
  result = etw.close(handle);
  ASSERT_EQ(result, etw.STATUS_ERROR);
}

#endif
