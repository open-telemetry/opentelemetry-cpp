// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/resource/resource_detector.h"

class CustomResourceDetector : public opentelemetry::sdk::resource::ResourceDetector
{
public:
  CustomResourceDetector(const std::string &comment) : comment_(comment) {}
  CustomResourceDetector(CustomResourceDetector &&)                      = delete;
  CustomResourceDetector(const CustomResourceDetector &)                 = delete;
  CustomResourceDetector &operator=(CustomResourceDetector &&)           = delete;
  CustomResourceDetector &operator=(const CustomResourceDetector &other) = delete;
  ~CustomResourceDetector() override                                     = default;

  opentelemetry::sdk::resource::Resource Detect() noexcept override;

private:
  std::string comment_;
};
