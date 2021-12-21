// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/multi_log_processor.h"

#  include <chrono>
#  include <memory>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

MultiLogProcessor::MultiLogProcessor(std::vector<std::unique_ptr<LogProcessor>> &&processors)
{
  for (auto &processor : processors)
  {
    AddProcessor(std::move(processor));
  }
}
MultiLogProcessor::~MultiLogProcessor()
{
  ForceFlush();
  Shutdown();
}

void MultiLogProcessor::AddProcessor(std::unique_ptr<LogProcessor> &&processor)
{
  // Add preocessor to end of the list.
  if (processor)
  {
    processors_.emplace_back(std::move(processor));
  }
}

std::unique_ptr<Recordable> MultiLogProcessor::MakeRecordable() noexcept
{
  auto recordable       = std::unique_ptr<Recordable>(new MultiRecordable);
  auto multi_recordable = static_cast<MultiRecordable *>(recordable.get());
  for (auto &processor : processors_)
  {
    multi_recordable->AddRecordable(*processor, processor->MakeRecordable());
  }
  return recordable;
}

void MultiLogProcessor::OnReceive(std::unique_ptr<Recordable> &&record) noexcept
{
  if (!record)
  {
    return;
  }
  auto multi_recordable = static_cast<MultiRecordable *>(record.get());

  for (auto &processor : processors_)
  {
    auto recordable = multi_recordable->ReleaseRecordable(*processor);
    if (recordable)
    {
      processor->OnReceive(std::move(record));
    }
  }
}

bool MultiLogProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  bool result      = true;
  auto start_time  = std::chrono::system_clock::now();
  auto expire_time = start_time + timeout;
  for (auto &processor : processors_)
  {
    if (!processor->ForceFlush(timeout))
    {
      result = false;
    }
    start_time = std::chrono::system_clock::now();
    if (expire_time > start_time)
    {
      timeout = std::chrono::duration_cast<std::chrono::microseconds>(expire_time - start_time);
    }
    else
    {
      timeout = std::chrono::microseconds::zero();
    }
  }
  return result;
}

bool MultiLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  bool result      = true;
  auto start_time  = std::chrono::system_clock::now();
  auto expire_time = start_time + timeout;
  for (auto &processor : processors_)
  {
    if (!processor->Shutdown(timeout))
    {
      result = false;
    }
    start_time = std::chrono::system_clock::now();
    if (expire_time > start_time)
    {
      timeout = std::chrono::duration_cast<std::chrono::microseconds>(expire_time - start_time);
    }
    else
    {
      timeout = std::chrono::microseconds::zero();
    }
  }
  return result;
}

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
