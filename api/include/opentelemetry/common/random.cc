#include "opentelemetry/common/random.h"
#include "opentelemetry/common/platform/fork.h"

#include <cstring>
#include <random>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace api
{
namespace common
{
// Wraps a thread_local random number generator, but adds a fork handler so that
// the generator will be correctly seeded after forking.
//
// See https://stackoverflow.com/q/51882689/4447365 and
//     https://github.com/opentracing-contrib/nginx-opentracing/issues/52
namespace
{
class TlsRandomNumberGenerator
{
public:
  TlsRandomNumberGenerator() noexcept
  {
    Seed();
    platform::AtFork(nullptr, nullptr, OnFork);
  }

  static FastRandomNumberGenerator &engine() noexcept { return engine_; }

private:
  static thread_local FastRandomNumberGenerator engine_;

  static void OnFork() noexcept { Seed(); }

  static void Seed() noexcept
  {
    std::random_device random_device;
    std::seed_seq seed_seq{random_device(), random_device(), random_device(), random_device()};
    engine_.seed(seed_seq);
  }
};

thread_local FastRandomNumberGenerator TlsRandomNumberGenerator::engine_{};
}  // namespace

FastRandomNumberGenerator &Random::GetRandomNumberGenerator() noexcept
{
  static thread_local TlsRandomNumberGenerator random_number_generator{};
  return TlsRandomNumberGenerator::engine();
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
}  // namespace api
OPENTELEMETRY_END_NAMESPACE
