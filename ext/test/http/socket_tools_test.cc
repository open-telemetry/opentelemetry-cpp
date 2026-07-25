// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/ext/http/server/socket_tools.h"

namespace
{

// A parsed address reports a family and a non-negative port; a rejected one reports port() == -1
// (its family is left AF_UNSPEC), which is how callers tell a real endpoint from a parse failure.

TEST(SocketAddrTest, ParsesHostAndPort)
{
  SocketTools::SocketAddr addr("127.0.0.1:8800");
  EXPECT_EQ(addr.port(), 8800);
  EXPECT_EQ(addr.toString(), "127.0.0.1:8800");
}

// The host part used to be copied into an uninitialized buffer that was terminated at a fixed
// index, so inet_pton read indeterminate bytes for any host shorter than 15 characters.
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

// A legitimate port 0 must be distinguishable from a parse failure, so this must parse rather
// than collapse to the same state the rejections use.
TEST(SocketAddrTest, ParsesLegitimateZeroPort)
{
  SocketTools::SocketAddr addr("127.0.0.1:0");
  EXPECT_EQ(addr.port(), 0);
  EXPECT_EQ(addr.toString(), "127.0.0.1:0");
}

TEST(SocketAddrTest, RejectsOutOfRangePort)
{
  SocketTools::SocketAddr addr("127.0.0.1:99999");
  EXPECT_EQ(addr.port(), -1);
}

// A host longer than the buffer must be rejected, not truncated into a different valid address:
// "255.255.255.2559" must not become "255.255.255.255".
TEST(SocketAddrTest, RejectsOverlongHostInsteadOfTruncating)
{
  SocketTools::SocketAddr addr("255.255.255.2559:80");
  EXPECT_EQ(addr.port(), -1);
}

TEST(SocketAddrTest, RejectsTrailingGarbageInPort)
{
  EXPECT_EQ(SocketTools::SocketAddr("127.0.0.1:80junk").port(), -1);
  EXPECT_EQ(SocketTools::SocketAddr("127.0.0.1:80:90").port(), -1);
}

TEST(SocketAddrTest, RejectsInvalidHost)
{
  EXPECT_EQ(SocketTools::SocketAddr("999.999.999.999:4318").port(), -1);
  EXPECT_EQ(SocketTools::SocketAddr("garbage").port(), -1);
}

// On Windows the copy loop was bounded by sizeof(buf), a byte count, rather than by the element
// count of a WCHAR array, so an address longer than 200 characters wrote past the end of the
// buffer. This must be handled rather than crash, and reported as a rejection.
TEST(SocketAddrTest, HandlesOverlongInput)
{
  const std::string overlong(512, '9');
  SocketTools::SocketAddr addr(overlong.c_str());
  EXPECT_EQ(addr.port(), -1);
}

TEST(SocketAddrTest, HandlesEmptyInput)
{
  SocketTools::SocketAddr addr("");
  EXPECT_EQ(addr.port(), -1);
}

}  // namespace
