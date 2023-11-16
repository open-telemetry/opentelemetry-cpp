
// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "src/common/random.h"
#include "src/common/platform/fork.h"

#include <cstring>
#include <random>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
// Wraps a thread_local random number generator.
// The previous fork handler is removed because it was not safe and was solving a problem that did
// not need to be solved since there should be no logic in the child() before it calls exec().

namespace
{
class TlsRandomNumberGenerator
{
public:
  TlsRandomNumberGenerator() noexcept
  {
    Seed();
  }

  FastRandomNumberGenerator & engine() noexcept { return engine_; }

private:
  FastRandomNumberGenerator engine_;

  void Seed() noexcept
  {
    std::random_device random_device;
    std::seed_seq seed_seq{random_device(), random_device(), random_device(), random_device()};
    engine_.seed(seed_seq);
  }
};

}  // namespace

FastRandomNumberGenerator &Random::GetRandomNumberGenerator() noexcept
{
  static thread_local TlsRandomNumberGenerator random_number_generator{};
  return random_number_generator.engine();
}

uint64_t Random::GenerateRandom64() noexcept
{
  return GetRandomNumberGenerator()();
}

void Random::GenerateRandomBuffer(opentelemetry::nostd::span<uint8_t> buffer) noexcept
{
  auto buf_size = buffer.size();

  for (size_t i = 0; i < buf_size; i += sizeof(uint64_t))
  {
    uint64_t value = GenerateRandom64();
    if (i + sizeof(uint64_t) <= buf_size)
    {
      memcpy(&buffer[i], &value, sizeof(uint64_t));
    }
    else
    {
      memcpy(&buffer[i], &value, buf_size - i);
    }
  }
}
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
