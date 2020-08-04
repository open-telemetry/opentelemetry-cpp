#include <gtest/gtest.h>
#include "opentelemetry/sdk/metrics/sync_instruments.h"

#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

TEST(Counter, InstrumentFunctions)
{
  Counter<int> alpha("enabled", "no description", "unitless", true);
  Counter<double> beta("not enabled", "some description", "units", false);

  EXPECT_EQ(static_cast<std::string>(alpha.GetName()), "enabled");
  EXPECT_EQ(static_cast<std::string>(alpha.GetDescription()), "no description");
  EXPECT_EQ(static_cast<std::string>(alpha.GetUnits()), "unitless");
  EXPECT_EQ(alpha.IsEnabled(), true);

  EXPECT_EQ(static_cast<std::string>(beta.GetName()), "not enabled");
  EXPECT_EQ(static_cast<std::string>(beta.GetDescription()), "some description");
  EXPECT_EQ(static_cast<std::string>(beta.GetUnits()), "units");
  EXPECT_EQ(beta.IsEnabled(), false);
}

TEST(Counter, Binding)
{
  Counter<int> alpha("test", "none", "unitless", true);

  std::map<std::string, std::string> labels  = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};
  std::map<std::string, std::string> labels2 = {{"key2", "value2"}, {"key3", "value3"}};
  std::map<std::string, std::string> labels3 = {{"key3", "value3"}, {"key2", "value2"}};

  auto labelkv  = trace::KeyValueIterableView<decltype(labels)>{labels};
  auto labelkv1 = trace::KeyValueIterableView<decltype(labels1)>{labels1};
  auto labelkv2 = trace::KeyValueIterableView<decltype(labels2)>{labels2};
  auto labelkv3 = trace::KeyValueIterableView<decltype(labels3)>{labels3};

  auto beta    = alpha.bindCounter(labelkv);
  auto gamma   = alpha.bindCounter(labelkv1);
  auto delta   = alpha.bindCounter(labelkv1);
  auto epsilon = alpha.bindCounter(labelkv1);
  auto zeta    = alpha.bindCounter(labelkv2);
  auto eta     = alpha.bindCounter(labelkv3);

  EXPECT_EQ(beta->get_ref(), 1);
  EXPECT_EQ(gamma->get_ref(), 3);
  EXPECT_EQ(eta->get_ref(), 2);

  delta->unbind();
  gamma->unbind();
  epsilon->unbind();

  EXPECT_EQ(alpha.boundInstruments_[KvToString(labelkv1)]->get_ref(), 0);
  EXPECT_EQ(alpha.boundInstruments_.size(), 3);
}

TEST(Counter, getAggsandnewupdate)
{
  Counter<int> alpha("test", "none", "unitless", true);

  std::map<std::string, std::string> labels  = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};

  // labels 2 and 3 are actually the same
  std::map<std::string, std::string> labels2 = {{"key2", "value2"}, {"key3", "value3"}};
  std::map<std::string, std::string> labels3 = {{"key3", "value3"}, {"key2", "value2"}};

  auto labelkv  = trace::KeyValueIterableView<decltype(labels)>{labels};
  auto labelkv1 = trace::KeyValueIterableView<decltype(labels1)>{labels1};
  auto labelkv2 = trace::KeyValueIterableView<decltype(labels2)>{labels2};
  auto labelkv3 = trace::KeyValueIterableView<decltype(labels3)>{labels3};

  auto beta    = alpha.bindCounter(labelkv);
  auto gamma   = alpha.bindCounter(labelkv1);
  auto delta   = alpha.bindCounter(labelkv1);
  auto epsilon = alpha.bindCounter(labelkv1);
  auto zeta    = alpha.bindCounter(labelkv2);
  auto eta     = alpha.bindCounter(labelkv3);

  EXPECT_EQ(beta->get_ref(), 1);
  EXPECT_EQ(gamma->get_ref(), 3);
  EXPECT_EQ(eta->get_ref(), 2);

  delta->unbind();
  gamma->unbind();
  epsilon->unbind();

  EXPECT_EQ(alpha.boundInstruments_[KvToString(labelkv1)]->get_ref(), 0);
  EXPECT_EQ(alpha.boundInstruments_.size(), 3);

  auto theta = alpha.GetRecords();
  EXPECT_EQ(theta.size(), 3);
  EXPECT_EQ(theta[0].GetName(), "test");
  EXPECT_EQ(theta[0].GetDescription(), "none");
  EXPECT_EQ(theta[0].GetLabels(), "{\"key2\":\"value2\",\"key3\":\"value3\"}");
  EXPECT_EQ(theta[1].GetLabels(), "{\"key1\":\"value1\"}");
}

void CounterCallback(std::shared_ptr<Counter<int>> in,
                     int freq,
                     const trace::KeyValueIterable &labels)
{
  for (int i = 0; i < freq; i++)
  {
    in->add(1, labels);
  }
}

TEST(Counter, StressAdd)
{
  std::shared_ptr<Counter<int>> alpha(new Counter<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels  = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};

  auto labelkv  = trace::KeyValueIterableView<decltype(labels)>{labels};
  auto labelkv1 = trace::KeyValueIterableView<decltype(labels1)>{labels1};

  std::thread first(CounterCallback, alpha, 100000, labelkv);
  std::thread second(CounterCallback, alpha, 100000, labelkv);
  std::thread third(CounterCallback, alpha, 300000, labelkv1);

  first.join();
  second.join();
  third.join();

  EXPECT_EQ(dynamic_cast<BoundCounter<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
                ->GetAggregator()
                ->get_values()[0],
            200000);
  EXPECT_EQ(dynamic_cast<BoundCounter<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
                ->GetAggregator()
                ->get_values()[0],
            300000);
}

void UpDownCounterCallback(std::shared_ptr<UpDownCounter<int>> in,
                           int freq,
                           const trace::KeyValueIterable &labels)
{
  for (int i = 0; i < freq; i++)
  {
    in->add(1, labels);
  }
}

void NegUpDownCounterCallback(std::shared_ptr<UpDownCounter<int>> in,
                              int freq,
                              const trace::KeyValueIterable &labels)
{
  for (int i = 0; i < freq; i++)
  {
    in->add(-1, labels);
  }
}

TEST(IntUpDownCounter, StressAdd)
{
  std::shared_ptr<UpDownCounter<int>> alpha(
      new UpDownCounter<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels  = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};
  auto labelkv  = trace::KeyValueIterableView<decltype(labels)>{labels};
  auto labelkv1 = trace::KeyValueIterableView<decltype(labels1)>{labels1};

  std::thread first(UpDownCounterCallback, alpha, 123400,
                    labelkv);  // spawn new threads that call the callback
  std::thread second(UpDownCounterCallback, alpha, 123400, labelkv);
  std::thread third(UpDownCounterCallback, alpha, 567800, labelkv1);
  std::thread fourth(NegUpDownCounterCallback, alpha, 123400, labelkv1);  // negative values

  first.join();
  second.join();
  third.join();
  fourth.join();

  EXPECT_EQ(
      dynamic_cast<BoundUpDownCounter<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
          ->GetAggregator()
          ->get_values()[0],
      123400 * 2);
  EXPECT_EQ(
      dynamic_cast<BoundUpDownCounter<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
          ->GetAggregator()
          ->get_values()[0],
      567800 - 123400);
}

void RecorderCallback(std::shared_ptr<ValueRecorder<int>> in,
                      int freq,
                      const trace::KeyValueIterable &labels)
{
  for (int i = 0; i < freq; i++)
  {
    in->record(i, labels);
  }
}

void NegRecorderCallback(std::shared_ptr<ValueRecorder<int>> in,
                         int freq,
                         const trace::KeyValueIterable &labels)
{
  for (int i = 0; i < freq; i++)
  {
    in->record(-i, labels);
  }
}

TEST(IntValueRecorder, StressRecord)
{
  std::shared_ptr<ValueRecorder<int>> alpha(
      new ValueRecorder<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels  = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};
  auto labelkv  = trace::KeyValueIterableView<decltype(labels)>{labels};
  auto labelkv1 = trace::KeyValueIterableView<decltype(labels1)>{labels1};

  std::thread first(RecorderCallback, alpha, 25,
                    labelkv);  // spawn new threads that call the callback
  std::thread second(RecorderCallback, alpha, 50, labelkv);
  std::thread third(RecorderCallback, alpha, 25, labelkv1);
  std::thread fourth(NegRecorderCallback, alpha, 100, labelkv1);  // negative values

  first.join();
  second.join();
  third.join();
  fourth.join();

  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
          ->GetAggregator()
          ->get_values()[0],
      0);  // min
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
          ->GetAggregator()
          ->get_values()[1],
      49);  // max
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
          ->GetAggregator()
          ->get_values()[2],
      1525);  // sum
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv)].get())
          ->GetAggregator()
          ->get_values()[3],
      75);  // count

  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
          ->GetAggregator()
          ->get_values()[0],
      -99);  // min
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
          ->GetAggregator()
          ->get_values()[1],
      24);  // max
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
          ->GetAggregator()
          ->get_values()[2],
      -4650);  // sum
  EXPECT_EQ(
      dynamic_cast<BoundValueRecorder<int> *>(alpha->boundInstruments_[KvToString(labelkv1)].get())
          ->GetAggregator()
          ->get_values()[3],
      125);  // count
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
