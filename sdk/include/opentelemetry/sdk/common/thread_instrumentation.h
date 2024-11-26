// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{

class ThreadInstrumentation
{
public:
  ThreadInstrumentation()          = default;
  virtual ~ThreadInstrumentation() = default;

  virtual void OnStart()    = 0;
  virtual void OnEnd()      = 0;
  virtual void BeforeWait() = 0;
  virtual void AfterWait()  = 0;
  virtual void BeforeLoad() = 0;
  virtual void AfterLoad()  = 0;
};

}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
