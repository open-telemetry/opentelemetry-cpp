// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/ext/http/server/socket_tools.h"

namespace
{

TEST(SocketAddrTest, ParsesHostAndPort)
{
  SocketTools::SocketAddr addr("127.0.0.1:8800");
  EXPECT_EQ(addr.port(), 8800);
  EXPECT_EQ(addr.toString(), "127.0.0.1:8800");
}

// The host part used to be copied into an uninitialized buffer that was terminated at a
// fixed index, so inet_pton read indeterminate bytes for any host shorter than 15
// characters.
TEST(SocketAddrTest, ParsesHostShorterThanTheBuffer)
{
  SocketTools::SocketAddr addr("1.2.3.4:80");
  EXPECT_EQ(addr.port(), 80);
  EXPECT_EQ(addr.toString(), "1.2.3.4:80");
}

TEST(SocketAddrTest, ParsesLongestRepresentableHost)
{
  SocketTools::SocketAddr addr("255.255.255.255:65535");
  EXPECT_EQ(addr.port(), 65535);
  EXPECT_EQ(addr.toString(), "255.255.255.255:65535");
}

TEST(SocketAddrTest, ParsesHostWithoutPort)
{
  SocketTools::SocketAddr addr("10.0.0.1");
  EXPECT_EQ(addr.port(), 0);
  EXPECT_EQ(addr.toString(), "10.0.0.1:0");
}

TEST(SocketAddrTest, RejectsOutOfRangePort)
{
  SocketTools::SocketAddr addr("127.0.0.1:99999");
  EXPECT_EQ(addr.port(), 0);
}

// On Windows the copy loop was bounded by sizeof(buf), a byte count, rather than by the
// element count of a WCHAR array, so an address longer than 200 characters wrote past the
// end of the buffer.
TEST(SocketAddrTest, HandlesOverlongInput)
{
  const std::string overlong(512, '9');
  SocketTools::SocketAddr addr(overlong.c_str());
  EXPECT_EQ(addr.port(), 0);
}

TEST(SocketAddrTest, HandlesEmptyInput)
{
  SocketTools::SocketAddr addr("");
  EXPECT_EQ(addr.port(), 0);
}

}  // namespace
