#include "opentelemetry/trace/trace_state.h"

#include <gtest/gtest.h>

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

TEST(TraceStateTest, DefaultConstruction)
{
  TraceState s;
  opentelemetry::nostd::string_view return_val = "";
  EXPECT_FALSE(s.Get("missing_key", return_val));
  EXPECT_EQ(return_val, "");
  EXPECT_TRUE(s.Empty());
  EXPECT_EQ(s.Entries().size(), 0);
}

TEST(TraceStateTest, Set)
{
  TraceState s;
  opentelemetry::nostd::string_view key = "test_key";
  opentelemetry::nostd::string_view val = "test_value";
  s.Set(key, val);

  opentelemetry::nostd::string_view bad_key = "bad_key";
  opentelemetry::nostd::string_view null_val;
  // Since string_view data is null by default, this should be a no-op
  s.Set(bad_key, null_val);

  opentelemetry::nostd::span<TraceState::Entry> entries = s.Entries();
  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0].GetKey(), key);
  EXPECT_EQ(entries[0].GetValue(), val);
}

TEST(TraceStateTest, Get)
{
  TraceState s;
  const int kNumPairs                                 = 3;
  opentelemetry::nostd::string_view keys[kNumPairs]   = {"test_key_1", "test_key_2", "test_key_3"};
  opentelemetry::nostd::string_view values[kNumPairs] = {"test_val_1", "test_val_2", "test_val_3"};

  for (int i = 0; i < kNumPairs; i++)
  {
    s.Set(keys[i], values[i]);
  }

  opentelemetry::nostd::string_view return_val = "";

  for (int i = 0; i < kNumPairs; i++)
  {
    EXPECT_TRUE(s.Get(keys[i], return_val));
    EXPECT_EQ(return_val, values[i]);
    return_val = "";
  }

  EXPECT_FALSE(s.Get("fake_key", return_val));
  EXPECT_EQ(return_val, "");
}

TEST(TraceStateTest, Empty)
{
  TraceState s;
  EXPECT_TRUE(s.Empty());

  s.Set("test_key", "test_value");
  EXPECT_FALSE(s.Empty());
}

TEST(TraceStateTest, Entries)
{
  TraceState s;
  const int kNumPairs                                 = 3;
  opentelemetry::nostd::string_view keys[kNumPairs]   = {"test_key_1", "test_key_2", "test_key_3"};
  opentelemetry::nostd::string_view values[kNumPairs] = {"test_val_1", "test_val_2", "test_val_3"};

  for (int i = 0; i < kNumPairs; i++)
  {
    s.Set(keys[i], values[i]);
  }

  opentelemetry::nostd::span<TraceState::Entry> entries = s.Entries();
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
  TraceState s;
  const int kNumPairs                               = 3;
  opentelemetry::nostd::string_view key_string      = "test_key_1test_key_2test_key_3";
  opentelemetry::nostd::string_view val_string      = "test_val_1test_val_2test_val_3";
  opentelemetry::nostd::string_view keys[kNumPairs] = {
      key_string.substr(0, 10), key_string.substr(10, 10), key_string.substr(20, 10)};
  opentelemetry::nostd::string_view values[kNumPairs] = {
      val_string.substr(0, 10), val_string.substr(10, 10), val_string.substr(20, 10)};

  for (int i = 0; i < kNumPairs; i++)
  {
    s.Set(keys[i], values[i]);
  }

  opentelemetry::nostd::span<TraceState::Entry> entries = s.Entries();
  for (int i = 0; i < kNumPairs; i++)
  {
    EXPECT_EQ(entries[i].GetKey(), keys[i]);
    EXPECT_EQ(entries[i].GetValue(), values[i]);
  }
}
}  // namespace
