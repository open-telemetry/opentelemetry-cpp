// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/resource/entity.h"

using namespace opentelemetry::sdk::resource;
namespace nostd = opentelemetry::nostd;

TEST(EntityTest, ConstructAndGetters)
{
  ResourceAttributes identity    = {{"service.name", "my-app"}};
  ResourceAttributes description = {{"service.version", "1.0.0"}};
  const std::string schema_url   = "https://opentelemetry.io/schemas/1.21.0";

  Entity entity("service", identity, description, schema_url);

  EXPECT_EQ(entity.GetType(), "service");
  EXPECT_EQ(entity.GetSchemaURL(), schema_url);
  ASSERT_EQ(entity.GetIdentity().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(entity.GetIdentity().at("service.name")), "my-app");
  ASSERT_EQ(entity.GetDescription().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(entity.GetDescription().at("service.version")), "1.0.0");
}

TEST(EntityTest, DefaultEmptyDescriptionAndSchemaUrl)
{
  ResourceAttributes identity = {{"host.id", "H1"}};
  Entity entity("host", identity);

  EXPECT_EQ(entity.GetType(), "host");
  EXPECT_TRUE(entity.GetDescription().empty());
  EXPECT_TRUE(entity.GetSchemaURL().empty());
  EXPECT_TRUE(entity.IsValid());
}

TEST(EntityTest, OverlappingIdentityDescriptionKeyIdentityWins)
{
  ResourceAttributes identity    = {{"host.name", "from-identity"}};
  ResourceAttributes description = {{"host.name", "from-description"}, {"host.type", "machine"}};

  Entity entity("host", identity, description);

  ASSERT_EQ(entity.GetIdentity().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(entity.GetIdentity().at("host.name")), "from-identity");
  EXPECT_TRUE(entity.GetDescription().find("host.name") == entity.GetDescription().end());
  ASSERT_EQ(entity.GetDescription().size(), 1);
  EXPECT_EQ(nostd::get<std::string>(entity.GetDescription().at("host.type")), "machine");
}

TEST(EntityTest, EmptyTypeIsInvalid)
{
  ResourceAttributes identity = {{"service.name", "app"}};
  Entity entity("", identity);

  EXPECT_TRUE(entity.GetType().empty());
  EXPECT_FALSE(entity.GetIdentity().empty());
  EXPECT_FALSE(entity.IsValid());
}

TEST(EntityTest, EmptyIdentityIsInvalid)
{
  Entity entity("service", ResourceAttributes{});

  EXPECT_EQ(entity.GetType(), "service");
  EXPECT_TRUE(entity.GetIdentity().empty());
  EXPECT_FALSE(entity.IsValid());
}

TEST(EntityTest, ValidMinimalEntity)
{
  ResourceAttributes identity = {{"service.name", "minimal-app"}};
  Entity entity("service", identity);

  EXPECT_TRUE(entity.IsValid());
}

TEST(EntityTest, CopyAndAssignment)
{
  ResourceAttributes identity    = {{"service.name", "app"}};
  ResourceAttributes description = {{"service.version", "1.0.0"}};
  Entity original("service", identity, description, "https://opentelemetry.io/schemas/1.0.0");

  Entity copied(original);
  EXPECT_TRUE(copied == original);
  EXPECT_EQ(copied.GetType(), original.GetType());
  EXPECT_EQ(copied.GetSchemaURL(), original.GetSchemaURL());
  EXPECT_EQ(copied.GetIdentity(), original.GetIdentity());
  EXPECT_EQ(copied.GetDescription(), original.GetDescription());

  Entity assigned("host", ResourceAttributes{{"host.id", "H1"}});
  assigned = original;
  EXPECT_TRUE(assigned == original);

  Entity moved(std::move(copied));
  EXPECT_TRUE(moved == original);
}

TEST(EntityTest, Equality)
{
  ResourceAttributes identity    = {{"service.name", "app"}};
  ResourceAttributes description = {{"service.version", "1.0.0"}};
  const std::string schema_url   = "https://opentelemetry.io/schemas/1.0.0";

  Entity a("service", identity, description, schema_url);
  Entity b("service", identity, description, schema_url);
  EXPECT_TRUE(a == b);

  Entity different_type("host", identity, description, schema_url);
  EXPECT_FALSE(a == different_type);

  Entity different_identity("service", ResourceAttributes{{"service.name", "other"}}, description,
                            schema_url);
  EXPECT_FALSE(a == different_identity);

  Entity different_description("service", identity,
                               ResourceAttributes{{"service.version", "2.0.0"}}, schema_url);
  EXPECT_FALSE(a == different_description);

  Entity different_schema("service", identity, description,
                          "https://opentelemetry.io/schemas/1.1.0");
  EXPECT_FALSE(a == different_schema);
}

TEST(EntityTest, IdentityInt32EqualsInt64)
{
  Entity as_int32("process", ResourceAttributes{{"process.pid", std::int32_t{123}}});
  Entity as_int64("process", ResourceAttributes{{"process.pid", std::int64_t{123}}});

  EXPECT_TRUE(as_int32 == as_int64);
  EXPECT_EQ(nostd::get<std::int64_t>(as_int32.GetIdentity().at("process.pid")), std::int64_t{123});
  EXPECT_TRUE(nostd::holds_alternative<std::int64_t>(as_int32.GetIdentity().at("process.pid")));
}

TEST(EntityTest, IdentityUint32EqualsInt64)
{
  Entity as_uint32("process", ResourceAttributes{{"process.pid", std::uint32_t{123}}});
  Entity as_int64("process", ResourceAttributes{{"process.pid", std::int64_t{123}}});

  EXPECT_TRUE(as_uint32 == as_int64);
  EXPECT_TRUE(nostd::holds_alternative<std::int64_t>(as_uint32.GetIdentity().at("process.pid")));
}

TEST(EntityTest, IdentityUint64InRangeEqualsInt64)
{
  const std::uint64_t in_range =
      static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
  Entity as_uint64("process", ResourceAttributes{{"process.pid", in_range}});
  Entity as_int64("process",
                  ResourceAttributes{{"process.pid", std::numeric_limits<std::int64_t>::max()}});

  EXPECT_TRUE(as_uint64 == as_int64);
  EXPECT_TRUE(nostd::holds_alternative<std::int64_t>(as_uint64.GetIdentity().at("process.pid")));
}

TEST(EntityTest, IdentityUint64OverflowRemainsDistinct)
{
  const std::uint64_t overflow = std::numeric_limits<std::uint64_t>::max();
  Entity as_uint64("process", ResourceAttributes{{"process.pid", overflow}});
  Entity as_int64("process",
                  ResourceAttributes{{"process.pid", std::numeric_limits<std::int64_t>::max()}});
  Entity as_negative("process", ResourceAttributes{{"process.pid", std::int64_t{-1}}});

  EXPECT_TRUE(nostd::holds_alternative<std::uint64_t>(as_uint64.GetIdentity().at("process.pid")));
  EXPECT_EQ(nostd::get<std::uint64_t>(as_uint64.GetIdentity().at("process.pid")), overflow);
  EXPECT_FALSE(as_uint64 == as_int64);
  EXPECT_FALSE(as_uint64 == as_negative);
}

TEST(EntityTest, IdentityIntegerArrayNormalization)
{
  const std::array<std::int32_t, 3> ids32    = {1, 2, 3};
  const std::array<std::uint32_t, 3> ids_u32 = {1, 2, 3};
  const std::array<std::int64_t, 3> ids64    = {1, 2, 3};
  const std::array<std::uint64_t, 3> ids_u64 = {1, 2, 3};
  Entity as_int32("host", ResourceAttributes{{"ids", nostd::span<const std::int32_t>{ids32}}});
  Entity as_uint32("host", ResourceAttributes{{"ids", nostd::span<const std::uint32_t>{ids_u32}}});
  Entity as_int64("host", ResourceAttributes{{"ids", nostd::span<const std::int64_t>{ids64}}});
  Entity as_uint64_fit("host",
                       ResourceAttributes{{"ids", nostd::span<const std::uint64_t>{ids_u64}}});

  EXPECT_TRUE(as_int32 == as_int64);
  EXPECT_TRUE(as_uint32 == as_int64);
  EXPECT_TRUE(as_uint64_fit == as_int64);
  EXPECT_TRUE(
      nostd::holds_alternative<std::vector<std::int64_t>>(as_int32.GetIdentity().at("ids")));

  const std::uint64_t overflow                    = std::numeric_limits<std::uint64_t>::max();
  const std::array<std::uint64_t, 2> overflow_ids = {1, overflow};
  Entity overflow_array(
      "host", ResourceAttributes{{"ids", nostd::span<const std::uint64_t>{overflow_ids}}});
  EXPECT_TRUE(
      nostd::holds_alternative<std::vector<std::uint64_t>>(overflow_array.GetIdentity().at("ids")));
  EXPECT_FALSE(overflow_array == as_int64);
}

TEST(EntityTest, DescriptionIntegersAreNotNormalized)
{
  Entity identity("service", ResourceAttributes{{"service.name", "app"}},
                  ResourceAttributes{{"count", std::int32_t{7}}});
  EXPECT_TRUE(nostd::holds_alternative<std::int32_t>(identity.GetDescription().at("count")));
}
