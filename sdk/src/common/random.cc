#include "src/common/random.h"
#include "src/common/platform/fork.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
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

FastRandomNumberGenerator &GetRandomNumberGenerator() noexcept
{
  static thread_local TlsRandomNumberGenerator random_number_generator{};
  return TlsRandomNumberGenerator::engine();
}
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
