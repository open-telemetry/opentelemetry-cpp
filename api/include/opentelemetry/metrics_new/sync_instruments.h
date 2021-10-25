// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/common/key_value_iterable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics_new
{

class SynchronousInstrument
{};

template <class T>
class Counter : public SynchronousInstrument
{

public:
  /**
   * Add adds the value to the counter's sum
   *
   * @param value The increment amount. MUST be non-negative.
   */
  virtual void add(T value) noexcept = 0;

  /**
   * Add adds the value to the counter's sum. The attributes should contain
   * the keys and values to be associated with this value.  Counters only
   * accept positive valued updates.
   *
   * @param value The increment amount. MUST be non-negative.
   * @param attributes the set of attributes, as key-value pairs
   */

  virtual void add(T value, const common::KeyValueIterable &attributes) noexcept = 0;
};

/** A histogram instrument that records values. */

template <class T>
class Historgram : public SynchronousInstrument
{
public:
  /**
   * Records a value.
   *
   * @param value The increment amount. May be positive, negative or zero.
   */
  virtual void record(T value) noexcept = 0;

  /**
   * Records a value with a set of attributes.
   *
   * @param value The increment amount. May be positive, negative or zero.
   * @param attributes A set of attributes to associate with the count.
   */
  void record(T value, const common::KeyValueIterable &attributes) noexcept = 0;

}

/** An up-down-counter instrument that adds or reduce values. */

template <class T>
class UpDownCounter : public SynchronousInstrument
{
public:
  /**
   * Adds a value.
   *
   * @param value The amount of the measurement.
   */
  virtual void add(T value) noexcept = 0;

  /**
   * Add a value with a set of attributes.
   *
   * @param value The increment amount. May be positive, negative or zero.
   * @param attributes A set of attributes to associate with the count.
   */
  void add(T value, const common::KeyValueIterable &attributes) noexcept = 0;
};

}  // namespace metrics_new
OPENTELEMETRY_END_NAMESPACE
#endif
