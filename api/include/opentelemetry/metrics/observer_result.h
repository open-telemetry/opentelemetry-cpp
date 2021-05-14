// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "instrument.h"
#  include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/**
 * ObserverResult class is necessary for the callback recording asynchronous
 * instrument use.  Callback functions asynchronous instruments are designed to
 * accept a single ObserverResult object and update using its pointer to the
 * instrument itself.
 */

template <class T>
class ObserverResult
{

public:
  ObserverResult() = default;

  ObserverResult(AsynchronousInstrument<T> *instrument) : instrument_(instrument) {}

  virtual void observe(T value, const common::KeyValueIterable &labels)
  {
    instrument_->observe(value, labels);
  }

private:
  AsynchronousInstrument<T> *instrument_;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
