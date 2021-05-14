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
#  include "observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class ValueObserver : virtual public AsynchronousInstrument<T>
{

public:
  ValueObserver() = default;

  ValueObserver(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit,
                bool enabled,
                void (*callback)(ObserverResult<T>))
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

  /**
   * Captures data by activating the callback function associated with the
   * instrument and storing its return value.  Callbacks for asynchronous
   * instruments are defined during construction.
   *
   * @param none
   * @return none
   */
  virtual void run() override = 0;
};

template <class T>
class SumObserver : virtual public AsynchronousInstrument<T>
{

public:
  SumObserver() = default;

  SumObserver(nostd::string_view name,
              nostd::string_view description,
              nostd::string_view unit,
              bool enabled,
              void (*callback)(ObserverResult<T>))
  {}

  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

  virtual void run() override = 0;
};

template <class T>
class UpDownSumObserver : virtual public AsynchronousInstrument<T>
{

public:
  UpDownSumObserver() = default;

  UpDownSumObserver(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled,
                    void (*callback)(ObserverResult<T>))
  {}

  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

  virtual void run() override = 0;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
