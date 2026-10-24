// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cmath>
#include <memory>
#include <string>

#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/sampler_env.h"
#include "opentelemetry/sdk/trace/samplers/always_off_factory.h"
#include "opentelemetry/sdk/trace/samplers/always_on_factory.h"
#include "opentelemetry/sdk/trace/samplers/parent_factory.h"
#include "opentelemetry/sdk/trace/samplers/trace_id_ratio_factory.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

namespace sampler_env
{

namespace
{

// Case-insensitive comparison of two ASCII strings
static bool CaseInsensitiveAsciiEquals(const std::string &lhs, const std::string &rhs) noexcept
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); ++i)
  {
    char a = lhs[i];
    char b = rhs[i];
    if (a >= 'A' && a <= 'Z')
      a += 32;
    if (b >= 'A' && b <= 'Z')
      b += 32;
    if (a != b)
    {
      return false;
    }
  }
  return true;
}

}  // namespace

static constexpr const char *kTracesSamplerEnv    = "OTEL_TRACES_SAMPLER";
static constexpr const char *kTracesSamplerArgEnv = "OTEL_TRACES_SAMPLER_ARG";
static constexpr double kDefaultTraceIdRatio      = 1.0;

std::unique_ptr<Sampler> GetSamplerFromEnv()
{
  std::string sampler_name;
  bool has_sampler_env =
      opentelemetry::sdk::common::GetStringEnvironmentVariable(kTracesSamplerEnv, sampler_name);

  if (!has_sampler_env || sampler_name.empty())
  {
    // When unset, preserve current default behavior (AlwaysOnSampler)
    return AlwaysOnSamplerFactory::Create();
  }

  float ratio_arg = kDefaultTraceIdRatio;
  bool has_ratio_env =
      opentelemetry::sdk::common::GetFloatEnvironmentVariable(kTracesSamplerArgEnv, ratio_arg);

  // For traceidratio and parentbased_traceidratio, default to 1.0 if arg is unset
  if (!has_ratio_env)
  {
    ratio_arg = static_cast<float>(kDefaultTraceIdRatio);
  }

  // Build sampler based on environment variable value
  if (CaseInsensitiveAsciiEquals(sampler_name, "always_on"))
  {
    return AlwaysOnSamplerFactory::Create();
  }
  else if (CaseInsensitiveAsciiEquals(sampler_name, "always_off"))
  {
    return AlwaysOffSamplerFactory::Create();
  }
  else if (CaseInsensitiveAsciiEquals(sampler_name, "traceidratio"))
  {
    // Validate ratio
    if (std::isnan(ratio_arg) || std::isinf(ratio_arg) || ratio_arg < 0.0f || ratio_arg > 1.0f)
    {
      OTEL_INTERNAL_LOG_WARN("Environment variable <" << kTracesSamplerArgEnv
                                                      << "> has invalid value <" << ratio_arg
                                                      << "> for traceidratio sampler, "
                                                      << "falling back to AlwaysOnSampler");
      return AlwaysOnSamplerFactory::Create();
    }
    return TraceIdRatioBasedSamplerFactory::Create(ratio_arg);
  }
  else if (CaseInsensitiveAsciiEquals(sampler_name, "parentbased_traceidratio"))
  {
    // Validate ratio - if invalid, use default ratio of 1.0
    if (std::isnan(ratio_arg) || std::isinf(ratio_arg) || ratio_arg < 0.0f || ratio_arg > 1.0f)
    {
      OTEL_INTERNAL_LOG_WARN("Environment variable <" << kTracesSamplerArgEnv
                                                      << "> has invalid value <" << ratio_arg
                                                      << "> for parentbased_traceidratio sampler, "
                                                      << "using default ratio of 1.0");
      ratio_arg = static_cast<float>(kDefaultTraceIdRatio);
    }
    auto root_sampler = TraceIdRatioBasedSamplerFactory::Create(ratio_arg);
    return ParentBasedSamplerFactory::Create(std::shared_ptr<Sampler>(root_sampler.release()));
  }
  else if (CaseInsensitiveAsciiEquals(sampler_name, "parentbased_always_on"))
  {
    auto root_sampler = AlwaysOnSamplerFactory::Create();
    return ParentBasedSamplerFactory::Create(std::shared_ptr<Sampler>(root_sampler.release()));
  }
  else if (CaseInsensitiveAsciiEquals(sampler_name, "parentbased_always_off"))
  {
    auto root_sampler = AlwaysOffSamplerFactory::Create();
    return ParentBasedSamplerFactory::Create(std::shared_ptr<Sampler>(root_sampler.release()));
  }
  else
  {
    // Unknown sampler name
    OTEL_INTERNAL_LOG_WARN("Environment variable <" << kTracesSamplerEnv << "> has unknown value <"
                                                    << sampler_name
                                                    << ">, falling back to AlwaysOnSampler");
    return AlwaysOnSamplerFactory::Create();
  }
}

}  // namespace sampler_env

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE