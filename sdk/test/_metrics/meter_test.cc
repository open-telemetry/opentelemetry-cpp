// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <future>

#  include "opentelemetry/sdk/_metrics/meter.h"

using namespace opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace common      = opentelemetry::common;
namespace nostd       = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE

TEST(Meter, CreateSyncInstruments)
{
  // Test that there are no errors creating synchronous instruments.
  Meter m("Test");

  m.NewShortCounter("Test-short-counter", "For testing", "Unitless", true);
  m.NewIntCounter("Test-int-counter", "For testing", "Unitless", true);
  m.NewFloatCounter("Test-float-counter", "For testing", "Unitless", true);
  m.NewDoubleCounter("Test-double-counter", "For testing", "Unitless", true);

  m.NewShortUpDownCounter("Test-short-ud-counter", "For testing", "Unitless", true);
  m.NewIntUpDownCounter("Test-int-ud-counter", "For testing", "Unitless", true);
  m.NewFloatUpDownCounter("Test-float-ud-counter", "For testing", "Unitless", true);
  m.NewDoubleUpDownCounter("Test-double-ud-counter", "For testing", "Unitless", true);

  m.NewShortValueRecorder("Test-short-recorder", "For testing", "Unitless", true);
  m.NewIntValueRecorder("Test-int-recorder", "For testing", "Unitless", true);
  m.NewFloatValueRecorder("Test-float-recorder", "For testing", "Unitless", true);
  m.NewDoubleValueRecorder("Test-double-recorder", "For testing", "Unitless", true);
}

// Dummy functions for asynchronous instrument constructors
void ShortCallback(metrics_api::ObserverResult<short>) {}
void IntCallback(metrics_api::ObserverResult<int>) {}
void FloatCallback(metrics_api::ObserverResult<float>) {}
void DoubleCallback(metrics_api::ObserverResult<double>) {}

TEST(Meter, CreateAsyncInstruments)
{
  // Test that there are no errors when creating asynchronous instruments.
  Meter m("Test");

  m.NewShortSumObserver("Test-short-sum-obs", "For testing", "Unitless", true, &ShortCallback);
  m.NewIntSumObserver("Test-int-sum-obs", "For testing", "Unitless", true, &IntCallback);
  m.NewFloatSumObserver("Test-float-sum-obs", "For testing", "Unitless", true, &FloatCallback);
  m.NewDoubleSumObserver("Test-double-sum-obs", "For testing", "Unitless", true, &DoubleCallback);

  m.NewShortUpDownSumObserver("Test-short-ud-sum-obs", "For testing", "Unitless", true,
                              &ShortCallback);
  m.NewIntUpDownSumObserver("Test-int-ud-sum-obs", "For testing", "Unitless", true, &IntCallback);
  m.NewFloatUpDownSumObserver("Test-float-ud-sum-obs", "For testing", "Unitless", true,
                              &FloatCallback);
  m.NewDoubleUpDownSumObserver("Test-double-ud-sum-obs", "For testing", "Unitless", true,
                               &DoubleCallback);

  m.NewShortValueObserver("Test-short-val-obs", "For testing", "Unitless", true, &ShortCallback);
  m.NewIntValueObserver("Test-int-val-obs", "For testing", "Unitless", true, &IntCallback);
  m.NewFloatValueObserver("Test-float-val-obs", "For testing", "Unitless", true, &FloatCallback);
  m.NewDoubleValueObserver("Test-double-val-obs", "For testing", "Unitless", true, &DoubleCallback);
}

TEST(Meter, CollectSyncInstruments)
{
  // Verify that the records returned on a call to Collect() are correct for synchronous
  // instruments.
  Meter m("Test");

  ASSERT_EQ(m.Collect().size(), 0);

  auto counter = m.NewShortCounter("Test-counter", "For testing", "Unitless", true);

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  counter->add(1, labelkv);

  std::vector<Record> res = m.Collect();
  auto agg_var            = res[0].GetAggregator();
  auto agg                = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 1);

  // Now call add() and Collect() again to ensure that the value in the underlying
  // aggregator was reset to the default.

  counter->add(10, labelkv);

  res     = m.Collect();
  agg_var = res[0].GetAggregator();
  agg     = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 10);
}

TEST(Meter, CollectDeletedSync)
{
  // Verify that calling Collect() after creating a synchronous instrument and destroying
  // the return pointer does not result in a segfault.

  Meter m("Test");

  ASSERT_EQ(m.Collect().size(), 0);

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  {
    auto counter = m.NewShortCounter("Test-counter", "For testing", "Unitless", true);
    counter->add(1, labelkv);
  }  // counter shared_ptr deleted here

  std::vector<Record> res = m.Collect();
  auto agg_var            = res[0].GetAggregator();
  auto agg                = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 1);
}

// Dummy function for asynchronous instrument constructors.
void Callback(metrics_api::ObserverResult<short> result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1, labelkv);
}

TEST(Meter, CollectAsyncInstruments)
{
  // Verify that the records returned on a call to Collect() are correct for asynchronous
  // instruments.
  Meter m("Test");

  ASSERT_EQ(m.Collect().size(), 0);

  auto sumobs =
      m.NewShortSumObserver("Test-counter", "For testing", "Unitless", true, &ShortCallback);

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  sumobs->observe(1, labelkv);

  std::vector<Record> res = m.Collect();
  auto agg_var            = res[0].GetAggregator();
  auto agg                = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 1);

  // Now call observe() and Collect() again to ensure that the value in the underlying
  // aggregator was reset to the default.

  sumobs->observe(10, labelkv);

  res     = m.Collect();
  agg_var = res[0].GetAggregator();
  agg     = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 10);
}

TEST(Meter, CollectDeletedAsync)
{
  // Verify that calling Collect() after creating an asynchronous instrument and destroying
  // the return pointer does not result in a segfault.

  Meter m("Test");

  ASSERT_EQ(m.Collect().size(), 0);

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  {
    auto sumobs = m.NewShortSumObserver("Test-counter", "For testing", "Unitless", true, &Callback);
    sumobs->observe(1, labelkv);
  }  // sumobs shared_ptr deleted here

  std::vector<Record> res = m.Collect();
  auto agg_var            = res[0].GetAggregator();
  auto agg                = nostd::get<0>(agg_var);

  ASSERT_EQ(agg->get_checkpoint()[0], 1);
}

TEST(Meter, RecordBatch)
{
  // This tests that RecordBatch appropriately updates the aggregators of the instruments
  // passed to the function. Short, int, float, and double data types are tested.
  Meter m("Test");

  auto scounter = m.NewShortCounter("Test-scounter", "For testing", "Unitless", true);
  auto icounter = m.NewIntCounter("Test-icounter", "For testing", "Unitless", true);
  auto fcounter = m.NewFloatCounter("Test-fcounter", "For testing", "Unitless", true);
  auto dcounter = m.NewDoubleCounter("Test-dcounter", "For testing", "Unitless", true);

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  metrics_api::SynchronousInstrument<short> *sinstr_arr[] = {scounter.get()};
  short svalues_arr[]                                     = {1};

  nostd::span<metrics_api::SynchronousInstrument<short> *> sinstrs{sinstr_arr};
  nostd::span<const short, 1> svalues{svalues_arr};

  m.RecordShortBatch(labelkv, sinstrs, svalues);
  std::vector<Record> res = m.Collect();
  auto short_agg_var      = res[0].GetAggregator();
  auto short_agg          = nostd::get<0>(short_agg_var);
  ASSERT_EQ(short_agg->get_checkpoint()[0], 1);

  metrics_api::SynchronousInstrument<int> *iinstr_arr[] = {icounter.get()};
  int ivalues_arr[]                                     = {1};

  nostd::span<metrics_api::SynchronousInstrument<int> *> iinstrs{iinstr_arr};
  nostd::span<const int, 1> ivalues{ivalues_arr};

  m.RecordIntBatch(labelkv, iinstrs, ivalues);
  res              = m.Collect();
  auto int_agg_var = res[0].GetAggregator();
  auto int_agg     = nostd::get<1>(int_agg_var);
  ASSERT_EQ(int_agg->get_checkpoint()[0], 1);

  metrics_api::SynchronousInstrument<float> *finstr_arr[] = {fcounter.get()};
  float fvalues_arr[]                                     = {1.0};

  nostd::span<metrics_api::SynchronousInstrument<float> *> finstrs{finstr_arr};
  nostd::span<const float, 1> fvalues{fvalues_arr};

  m.RecordFloatBatch(labelkv, finstrs, fvalues);
  res                = m.Collect();
  auto float_agg_var = res[0].GetAggregator();
  auto float_agg     = nostd::get<2>(float_agg_var);
  ASSERT_EQ(float_agg->get_checkpoint()[0], 1.0);

  metrics_api::SynchronousInstrument<double> *dinstr_arr[] = {dcounter.get()};
  double dvalues_arr[]                                     = {1.0};

  nostd::span<metrics_api::SynchronousInstrument<double> *> dinstrs{dinstr_arr};
  nostd::span<const double, 1> dvalues{dvalues_arr};

  m.RecordDoubleBatch(labelkv, dinstrs, dvalues);
  res                 = m.Collect();
  auto double_agg_var = res[0].GetAggregator();
  auto double_agg     = nostd::get<3>(double_agg_var);
  ASSERT_EQ(double_agg->get_checkpoint()[0], 1.0);
}

TEST(Meter, DisableCollectSync)
{
  Meter m("Test");
  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  auto c       = m.NewShortCounter("c", "", "", false);
  c->add(1, labelkv);
  ASSERT_EQ(m.Collect().size(), 0);
}

TEST(Meter, DisableCollectAsync)
{
  Meter m("Test");
  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  auto c       = m.NewShortValueObserver("c", "", "", false, &ShortCallback);
  c->observe(1, labelkv);
  ASSERT_EQ(m.Collect().size(), 0);
}

TEST(MeterStringUtil, IsValid)
{
#  if __EXCEPTIONS
  Meter m("Test");
  ASSERT_ANY_THROW(m.NewShortCounter("", "Empty name is invalid", " ", true));
  ASSERT_ANY_THROW(m.NewShortCounter("1a", "Can't begin with a number", " ", true));
  ASSERT_ANY_THROW(m.NewShortCounter(".a", "Can't begin with punctuation", " ", true));
  ASSERT_ANY_THROW(m.NewShortCounter(" a", "Can't begin with space", " ", true));
  ASSERT_ANY_THROW(m.NewShortCounter(
      "te^ s=%t", "Only alphanumeric ., -, and _ characters are allowed", " ", true));
#  endif
}

TEST(MeterStringUtil, AlreadyExists)
{
#  if __EXCEPTIONS
  Meter m("Test");

  m.NewShortCounter("a", "First instance of instrument named 'a'", "", true);
  ASSERT_ANY_THROW(m.NewShortCounter("a", "Second (illegal) instrument named 'a'", "", true));
  ASSERT_ANY_THROW(m.NewShortSumObserver("a", "Still illegal even though it is not a short counter",
                                         "", true, &ShortCallback));
#  endif
}
OPENTELEMETRY_END_NAMESPACE
#endif
