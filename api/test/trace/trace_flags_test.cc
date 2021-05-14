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

#include "opentelemetry/trace/trace_flags.h"

#include <cstring>
#include <string>

#include <gtest/gtest.h>

namespace
{

using opentelemetry::trace::TraceFlags;

std::string Hex(const TraceFlags &flags)
{
  char buf[2];
  flags.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}

TEST(TraceFlagsTest, DefaultConstruction)
{
  TraceFlags flags;
  EXPECT_FALSE(flags.IsSampled());
  EXPECT_EQ(0, flags.flags());
  EXPECT_EQ("00", Hex(flags));
}

TEST(TraceFlagsTest, Sampled)
{
  TraceFlags flags{TraceFlags::kIsSampled};
  EXPECT_TRUE(flags.IsSampled());
  EXPECT_EQ(1, flags.flags());
  EXPECT_EQ("01", Hex(flags));

  uint8_t buf[1];
  flags.CopyBytesTo(buf);
  EXPECT_EQ(1, buf[0]);
}

}  // namespace
