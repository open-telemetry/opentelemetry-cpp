// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>
#ifndef _WIN32
#  include <sys/socket.h>  // for sockaddr, AF_UNSPEC, AF_INET
#endif

#include "opentelemetry/ext/http/server/socket_tools.h"

namespace
{

// A parsed address reports AF_INET and a non-negative port; a rejected one is left at AF_UNSPEC
// with port() == -1. Checking the family too, not just the port, catches a family byte being
// corrupted while port() happens to still return -1.
void ExpectInvalid(const SocketTools::SocketAddr &addr)
{
  EXPECT_EQ(addr.m_data.sa_family, AF_UNSPEC);
  EXPECT_EQ(addr.port(), -1);
}

TEST(SocketAddrTest, ParsesHostAndPort)
{
  SocketTools::SocketAddr addr("127.0.0.1:8800");
  EXPECT_EQ(addr.m_data.sa_family, AF_INET);
  EXPECT_EQ(addr.port(), 8800);
  EXPECT_EQ(addr.toString(), "127.0.0.1:8800");
}

// A host shorter than the buffer must be null-terminated at its actual length; terminating at a
// fixed index would leave inet_pton reading indeterminate bytes for any host under 15 characters.
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
  EXPECT_EQ(addr.m_data.sa_family, AF_INET);
  EXPECT_EQ(addr.port(), 0);
  EXPECT_EQ(addr.toString(), "127.0.0.1:0");
}

// A decimal port may carry leading zeros; the value is what matters, so "080" is port 80. The
// pre-multiply overflow check still bounds the value at 65535 no matter how many zeros precede it.
TEST(SocketAddrTest, AcceptsLeadingZeroPort)
{
  SocketTools::SocketAddr addr("127.0.0.1:080");
  EXPECT_EQ(addr.m_data.sa_family, AF_INET);
  EXPECT_EQ(addr.port(), 80);
}

TEST(SocketAddrTest, RejectsOutOfRangePort)
{
  SocketTools::SocketAddr addr("127.0.0.1:99999");
  ExpectInvalid(addr);
}

// A host longer than the buffer must be rejected, not truncated into a different valid address:
// "255.255.255.2559" must not become "255.255.255.255".
TEST(SocketAddrTest, RejectsOverlongHostInsteadOfTruncating)
{
  SocketTools::SocketAddr addr("255.255.255.2559:80");
  ExpectInvalid(addr);
}

TEST(SocketAddrTest, RejectsTrailingGarbageInPort)
{
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:80junk"));
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:80:90"));
}

TEST(SocketAddrTest, RejectsInvalidHost)
{
  ExpectInvalid(SocketTools::SocketAddr("999.999.999.999:4318"));
  ExpectInvalid(SocketTools::SocketAddr("garbage"));
}

// A host far longer than the 15-character dotted-quad maximum must be rejected by the length
// bound before it can drive an out-of-bounds access on the fixed host buffer.
TEST(SocketAddrTest, RejectsExtremelyOverlongHost)
{
  const std::string overlong(512, '9');
  SocketTools::SocketAddr addr(overlong.c_str());
  ExpectInvalid(addr);
}

TEST(SocketAddrTest, HandlesEmptyInput)
{
  SocketTools::SocketAddr addr("");
  ExpectInvalid(addr);
}

TEST(SocketAddrTest, RejectsNullInput)
{
  SocketTools::SocketAddr addr(nullptr);
  ExpectInvalid(addr);
}

TEST(SocketAddrTest, RejectsEmptyHostOrPort)
{
  ExpectInvalid(SocketTools::SocketAddr(":80"));
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:"));
}

// The port grammar is decimal digits only: a sign or whitespace that strtol would have accepted
// must be rejected so both platforms agree.
TEST(SocketAddrTest, RejectsSignAndWhitespaceInPort)
{
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:+80"));
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:-0"));
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1: 80"));
}

TEST(SocketAddrTest, RejectsPortOverflow)
{
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:65536"));
  ExpectInvalid(SocketTools::SocketAddr("127.0.0.1:4294967377"));
}

// inet_pton() requires four decimal components, so it rejects the shorthand form ("127.1") that the
// legacy inet_aton()/inet_addr() resolvers accepted. This holds on every platform (verified on
// glibc and macOS). Leading-zero components ("01.02.03.004") are deliberately not asserted: glibc
// rejects them but macOS inet_pton accepts them, so that outcome is platform-dependent.
TEST(SocketAddrTest, RejectsShorthandHost)
{
  ExpectInvalid(SocketTools::SocketAddr("127.1:80"));
}

}  // namespace
