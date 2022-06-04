// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"

#include <cstring>
#include <random>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
namespace internal_log
{

LogHandler::~LogHandler() {}

void DefaultLogHandler::Handle(LogLevel level,
                               const char *file,
                               int line,
                               const char *msg,
                               const sdk::common::AttributeMap &attributes) noexcept
{
  std::stringstream output_s;
  output_s << "[" << LevelToString(level) << "] ";
  if (file != nullptr)
  {
    output_s << "File: " << file << ":" << line;
  }
  if (msg != nullptr)
  {
    output_s << msg;
  }
  output_s << std::endl;
  // TBD - print attributes
  std::cout << output_s.str();  // thread safe.
}

void NoopLogHandler::Handle(LogLevel,
                            const char *,
                            int,
                            const char *,
                            const sdk::common::AttributeMap &) noexcept
{}

std::pair<nostd::shared_ptr<LogHandler>, LogLevel> &GlobalLogHandler::GetHandlerAndLevel() noexcept
{
  static std::pair<nostd::shared_ptr<LogHandler>, LogLevel> handler_and_level{
      nostd::shared_ptr<LogHandler>(new DefaultLogHandler), LogLevel::Warning};
  return handler_and_level;
}

}  // namespace internal_log
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
