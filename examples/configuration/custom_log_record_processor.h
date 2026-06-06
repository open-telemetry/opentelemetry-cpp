// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <string>

#include "opentelemetry/sdk/logs/processor.h"

class CustomLogRecordProcessor : public opentelemetry::sdk::logs::LogRecordProcessor
{
public:
  CustomLogRecordProcessor(const std::string &comment) : comment_(comment) {}
  CustomLogRecordProcessor(CustomLogRecordProcessor &&)                      = delete;
  CustomLogRecordProcessor(const CustomLogRecordProcessor &)                 = delete;
  CustomLogRecordProcessor &operator=(CustomLogRecordProcessor &&)           = delete;
  CustomLogRecordProcessor &operator=(const CustomLogRecordProcessor &other) = delete;
  ~CustomLogRecordProcessor() override                                       = default;

  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override;

  void OnEmit(std::unique_ptr<opentelemetry::sdk::logs::Recordable> &&record) noexcept override;

  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

private:
  std::string comment_;
};
