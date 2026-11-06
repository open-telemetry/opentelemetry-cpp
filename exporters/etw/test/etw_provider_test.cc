// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef _WIN32

#  include <gtest/gtest.h>
#  include <array>
#  include <cstdint>
#  include <cstdio>
#  include <cstring>
#  include <ctime>
#  include <limits>
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

TEST(ETWProvider, BorrowedEventNameDoesNotModifyEitherMap)
{
  ETWProvider provider;
  auto handle = provider.open("BorrowedEventName", ETWProvider::ETW_MANIFEST);
  exporter::etw::Properties attributes{{ETW_FIELD_NAME, "borrowed"}};
  exporter::etw::Properties fields;
  EXPECT_EQ(
      provider.write(handle, fields, nullptr, nullptr, 0, ETWProvider::ETW_MANIFEST, attributes),
      provider.STATUS_OK);
  EXPECT_TRUE(fields.empty());
  ASSERT_EQ(attributes.size(), 1u);
  EXPECT_EQ(nostd::get<std::string>(attributes.at(ETW_FIELD_NAME)), "borrowed");
  provider.close(handle);
}

TEST(ETWProvider, TimestampFormatterPreservesPreviousRepresentation)
{
  auto original = [](int64_t timestamp) {
    char text[sizeof("YYYY-MM-DDTHH:MM:SS.sssssssssZ") + 1] = {};
    __time64_t seconds                                      = timestamp / 1000000000;
    const int fraction = static_cast<int>(timestamp % 1000000000);
    tm calendar;
    if (::_gmtime64_s(&calendar, &seconds) != 0)
    {
      std::memset(&calendar, 0, sizeof(calendar));
    }
    ::_snprintf_s(text, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02d.%09dZ", 1900 + calendar.tm_year,
                  1 + calendar.tm_mon, calendar.tm_mday, calendar.tm_hour, calendar.tm_min,
                  calendar.tm_sec, fraction);
    return std::string(text);
  };
  const std::array<int64_t, 12> edges{0,
                                      1,
                                      -1,
                                      999999999,
                                      1000000000,
                                      -1000000000,
                                      951782400123456789,
                                      1709251199999999999,
                                      1709251200000000000,
                                      1735689600000000000,
                                      (std::numeric_limits<int64_t>::min)(),
                                      (std::numeric_limits<int64_t>::max)()};
  for (const auto timestamp : edges)
  {
    EXPECT_EQ(utils::formatUtcTimestampNsAsISO8601(timestamp), original(timestamp)) << timestamp;
  }
  uint64_t random = 42;
  for (size_t i = 0; i < 10000; ++i)
  {
    random               = random * 6364136223846793005ULL + 1;
    const auto timestamp = static_cast<int64_t>(random >> 1);
    ASSERT_EQ(utils::formatUtcTimestampNsAsISO8601(timestamp), original(timestamp)) << timestamp;
    ASSERT_EQ(utils::formatUtcTimestampNsAsISO8601(-timestamp), original(-timestamp)) << -timestamp;
  }
}

#endif
