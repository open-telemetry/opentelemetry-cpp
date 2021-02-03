#include "opentelemetry/trace/trace_state.h"

#include <gtest/gtest.h>
#include "opentelemetry/nostd/string_view.h"

namespace
{

using opentelemetry::trace::TraceState;

// Random string of length 257. Used for testing strings with max length 256.
const char *kLongString =
    "4aekid3he76zgytjavudqqeltyvu5zqio2lx7d92dlxlf0z4883irvxuwelsq27sx1mlrjg3r7ad3jeq09rjppyd9veorg"
    "2nmihy4vilabfts8bsxruih0urusmjnglzl3iwpjinmo835dbojcrd73p56nw80v4xxrkye59ytmu5v84ysfa24d58ovv9"
    "w1n54n0mhhf4z0mpv6oudywrp9vfoks6lrvxv3uihvbi2ihazf237kvt1nbsjn3kdvfdb";

// ------------------------- Entry class tests ---------------------------------

// Test constructor that takes a key-value pair
TEST(EntryTest, KeyValueConstruction)
{
  opentelemetry::nostd::string_view key = "test_key";
  opentelemetry::nostd::string_view val = "test_value";
  TraceState::Entry e(key, val);

  EXPECT_EQ(key.size(), e.GetKey().size());
  EXPECT_EQ(key, e.GetKey());

  EXPECT_EQ(val.size(), e.GetValue().size());
  EXPECT_EQ(val, e.GetValue());
}

// Test copy constructor
TEST(EntryTest, Copy)
{
  TraceState::Entry e("test_key", "test_value");
  TraceState::Entry copy(e);
  EXPECT_EQ(copy.GetKey(), e.GetKey());
  EXPECT_EQ(copy.GetValue(), e.GetValue());
}

// Test assignment operator
TEST(EntryTest, Assignment)
{
  TraceState::Entry e("test_key", "test_value");
  TraceState::Entry empty;
  empty = e;
  EXPECT_EQ(empty.GetKey(), e.GetKey());
  EXPECT_EQ(empty.GetValue(), e.GetValue());
}

TEST(EntryTest, SetValue)
{
  TraceState::Entry e("test_key", "test_value");
  opentelemetry::nostd::string_view new_val = "new_value";
  e.SetValue(new_val);

  EXPECT_EQ(new_val.size(), e.GetValue().size());
  EXPECT_EQ(new_val, e.GetValue());
}

// -------------------------- TraceState class tests ---------------------------

std::string create_ts_return_header(std::string header)
{
  TraceState ts = TraceState::FromHeader(header);
  return ts.ToHeader();
}

std::string header_with_max_members()
{
  std::string header = "";
  auto max_members   = opentelemetry::trace::TraceState::kMaxKeyValuePairs;
  for (int i = 0; i < max_members; i++)
  {
    std::string key   = "key" + std::to_string(i);
    std::string value = "value" + std::to_string(i);
    header += key + "=" + value;
    if (i != max_members - 1)
    {
      header += ",";
    }
  }
  return header;
}

TEST(TraceStateTest, ValidateHeaderParsing)
{
  auto max_trace_state_header = header_with_max_members();

  struct
  {
    const char *input;
    const char *expected;
  } testcases[] = {{"k1=v1", "k1=v1"},
                   {"K1=V1", ""},
                   {"k1=v1,k2=v2,k3=v3", "k1=v1,k2=v2,k3=v3"},
                   {"k1=v1,k2=v2,,", "k1=v1,k2=v2"},
                   {"k1=v1,k2=v2,invalidmember", ""},
                   {"1a-2f@foo=bar1,a*/foo-_/bar=bar4", "1a-2f@foo=bar1,a*/foo-_/bar=bar4"},
                   {"1a-2f@foo=bar1,*/foo-_/bar=bar4", ""},
                   {",k1=v1", "k1=v1"},
                   {",", ""},
                   {",=,", ""},
                   {"", ""},
                   {max_trace_state_header.data(), max_trace_state_header.data()}};
  for (auto &testcase : testcases)
  {
    EXPECT_EQ(TraceState::FromHeader(testcase.input).ToHeader(), testcase.expected);
  }
}

TEST(TraceStateTest, TraceStateGet)
{

  std::string trace_state_header = header_with_max_members();
  TraceState ts                  = TraceState::FromHeader(trace_state_header);

  EXPECT_EQ(ts.Get("key0"), "value0");
  EXPECT_EQ(ts.Get("key16"), "value16");
  EXPECT_EQ(ts.Get("key31"), "value31");
  EXPECT_EQ(ts.Get("key32"), "");  // key not found
}

TEST(TraceStateTest, TraceStateSet)
{
  std::string trace_state_header = "k1=v1,k2=v2";
  auto ts1                       = TraceState::FromHeader(trace_state_header);
  auto ts1_new                   = ts1.Set("k3", "v3");
  EXPECT_EQ(ts1_new.ToHeader(), "k3=v3,k1=v1,k2=v2");

  trace_state_header = header_with_max_members();
  auto ts2           = TraceState::FromHeader(trace_state_header);
  auto ts2_new =
      ts2.Set("n_k1", "n_v1");  // adding to max list, should return copy of existing list
  EXPECT_EQ(ts2_new.ToHeader(), trace_state_header);

  trace_state_header = "k1=v1,k2=v2";
  auto ts3           = TraceState::FromHeader(trace_state_header);
  auto ts3_new       = ts3.Set("*n_k1", "n_v1");  // adding invalid key, should return empty
  EXPECT_EQ(ts3_new.ToHeader(), "");
}

TEST(TraceStateTest, TraceStateDelete)
{
  std::string trace_state_header = "k1=v1,k2=v2,k3=v3";
  auto ts1                       = TraceState::FromHeader(trace_state_header);
  auto ts1_new                   = ts1.Delete(std::string("k1"));
  EXPECT_EQ(ts1_new.ToHeader(), "k2=v2,k3=v3");

  trace_state_header = "k1=v1";  // single list member
  auto ts2           = TraceState::FromHeader(trace_state_header);
  auto ts2_new       = ts2.Delete(std::string("k1"));
  EXPECT_EQ(ts2_new.ToHeader(), "");

  trace_state_header = "k1=v1";  // single list member, delete invalid entry
  auto ts3           = TraceState::FromHeader(trace_state_header);
  auto ts3_new       = ts3.Delete(std::string("InvalidKey"));
  EXPECT_EQ(ts3_new.ToHeader(), "");
}

TEST(TraceStateTest, Empty)
{
  std::string trace_state_header = "";
  auto ts                        = TraceState::FromHeader(trace_state_header);
  EXPECT_TRUE(ts.Empty());

  trace_state_header = "k1=v1,k2=v2";
  auto ts1           = TraceState::FromHeader(trace_state_header);
  EXPECT_FALSE(ts1.Empty());
}

TEST(TraceStateTest, Entries)
{
  std::string trace_state_header                      = "k1=v1,k2=v2,k3=v3";
  auto ts1                                            = TraceState::FromHeader(trace_state_header);
  const int kNumPairs                                 = 3;
  opentelemetry::nostd::string_view keys[kNumPairs]   = {"k1", "k2", "k3"};
  opentelemetry::nostd::string_view values[kNumPairs] = {"v1", "v2", "v3"};

  opentelemetry::nostd::span<TraceState::Entry> entries = ts1.Entries();
  for (int i = 0; i < kNumPairs; i++)
  {
    EXPECT_EQ(entries[i].GetKey(), keys[i]);
    EXPECT_EQ(entries[i].GetValue(), values[i]);
  }
}

TEST(TraceStateTest, IsValidKey)
{
  EXPECT_TRUE(TraceState::IsValidKey("valid-key23/*"));
  EXPECT_FALSE(TraceState::IsValidKey("Invalid_key"));
  EXPECT_FALSE(TraceState::IsValidKey("invalid$Key&"));
  EXPECT_FALSE(TraceState::IsValidKey(""));
  EXPECT_FALSE(TraceState::IsValidKey(kLongString));
}

TEST(TraceStateTest, IsValidValue)
{
  EXPECT_TRUE(TraceState::IsValidValue("valid-val$%&~"));
  EXPECT_FALSE(TraceState::IsValidValue("\tinvalid"));
  EXPECT_FALSE(TraceState::IsValidValue("invalid="));
  EXPECT_FALSE(TraceState::IsValidValue("invalid,val"));
  EXPECT_FALSE(TraceState::IsValidValue(""));
  EXPECT_FALSE(TraceState::IsValidValue(kLongString));
}

// Tests that keys and values don't depend on null terminators
TEST(TraceStateTest, MemorySafe)
{
  std::string trace_state_header                    = "";
  auto ts                                           = TraceState::FromHeader(trace_state_header);
  const int kNumPairs                               = 3;
  opentelemetry::nostd::string_view key_string      = "test_key_1test_key_2test_key_3";
  opentelemetry::nostd::string_view val_string      = "test_val_1test_val_2test_val_3";
  opentelemetry::nostd::string_view keys[kNumPairs] = {
      key_string.substr(0, 10), key_string.substr(10, 10), key_string.substr(20, 10)};
  opentelemetry::nostd::string_view values[kNumPairs] = {
      val_string.substr(0, 10), val_string.substr(10, 10), val_string.substr(20, 10)};

  auto ts1 = ts.Set(keys[2], values[2]);
  auto ts2 = ts1.Set(keys[1], values[1]);
  auto ts3 = ts2.Set(keys[0], values[0]);

  opentelemetry::nostd::span<TraceState::Entry> entries = ts3.Entries();
  for (int i = 0; i < kNumPairs; i++)
  {
    EXPECT_EQ(entries[i].GetKey(), keys[i]);
    EXPECT_EQ(entries[i].GetValue(), values[i]);
  }
}
}  // namespace
