#include <gtest/gtest.h>
#include "opentelemetry/sdk/metrics/sync_instruments.h"
#include "opentelemetry/sdk/metrics/async_instruments.h"

#include <cstring>
#include <string>
#include <map>
#include <thread>
#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

TEST(Counter, InstrumentFunctions)
{
  Counter<int> alpha("enabled", "no description", "unitless", true);
  Counter<double> beta("not enabled", "some description", "units", false);

  EXPECT_EQ(alpha.GetName(), "enabled");
  EXPECT_EQ(alpha.GetDescription(), "no description");
  EXPECT_EQ(alpha.GetUnits(), "unitless");
  EXPECT_EQ(alpha.IsEnabled(), true);

  EXPECT_EQ(beta.GetName(), "not enabled");
  EXPECT_EQ(beta.GetDescription(), "some description");
  EXPECT_EQ(beta.GetUnits(), "units");
  EXPECT_EQ(beta.IsEnabled(), false);
}

TEST(Counter, Binding)
{
  Counter<int> alpha("test", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};
  std::map<std::string, std::string> labels2 = {{"key2", "value2"}, {"key3", "value3"}};
  std::map<std::string, std::string> labels3 = {{"key3", "value3"}, {"key2", "value2"}};

  auto beta = alpha.bind(labels);
  auto gamma = alpha.bind(labels1);
  auto delta = alpha.bind(labels1);
  auto epsilon = alpha.bind(labels1);
  auto zeta = alpha.bind(labels2);
  auto eta = alpha.bind(labels3);

  EXPECT_EQ (beta->get_ref(), 1);
  EXPECT_EQ(gamma->get_ref(),3);
  EXPECT_EQ(eta->get_ref(),2);

  delta->unbind();
  gamma->unbind();
  epsilon->unbind();

  EXPECT_EQ(alpha.boundInstruments_[mapToString(labels1)]->get_ref(), 0);
  EXPECT_EQ(alpha.boundInstruments_.size(), 3);
}

void CounterCallback(std::shared_ptr<Counter<int>> in, int freq, std::map<std::string, std::string> labels){
  for (int i=0; i<freq; i++){
    in->add(1, labels);
  }
}

TEST(Counter, StressAdd){
  std::shared_ptr<Counter<int>> alpha(new Counter<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};

  std::thread first (CounterCallback, alpha, 100000, labels);     
  std::thread second (CounterCallback, alpha, 100000, labels);
  std::thread third (CounterCallback, alpha, 300000, labels1);

  first.join();                
  second.join();
  third.join();

  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[0], 200000);
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[0], 300000);
}

void UpDownCounterCallback(std::shared_ptr<UpDownCounter<int>> in, int freq, std::map<std::string, std::string> labels){
  for (int i=0; i<freq; i++){
    in->add(1, labels);
  }
}

void NegUpDownCounterCallback(std::shared_ptr<UpDownCounter<int>> in, int freq, std::map<std::string, std::string> labels){
  for (int i=0; i<freq; i++){
    in->add(-1, labels);
  }
}

TEST(IntUpDownCounter, StressAdd){
  std::shared_ptr<UpDownCounter<int>> alpha(new UpDownCounter<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};

  std::thread first (UpDownCounterCallback, alpha, 123400, labels);     // spawn new threads that call the callback
  std::thread second (UpDownCounterCallback, alpha, 123400, labels);
  std::thread third (UpDownCounterCallback, alpha, 567800, labels1);
  std::thread fourth (NegUpDownCounterCallback, alpha, 123400, labels1); // negative values

  first.join();                
  second.join();               
  third.join();
  fourth.join();

  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[0], 123400*2);
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[0], 567800-123400);
}

void RecorderCallback(std::shared_ptr<ValueRecorder<int>> in, int freq, std::map<std::string, std::string> labels){
  for (int i=0; i<freq; i++){
    in->record(i, labels);
  }
}

void NegRecorderCallback(std::shared_ptr<ValueRecorder<int>> in, int freq, std::map<std::string, std::string> labels){
  for (int i=0; i<freq; i++){
    in->record(-i, labels);
  }
}

TEST(IntValueRecorder, StressRecord){
  std::shared_ptr<ValueRecorder<int>> alpha(new ValueRecorder<int>("test", "none", "unitless", true));

  std::map<std::string, std::string> labels = {{"key", "value"}};
  std::map<std::string, std::string> labels1 = {{"key1", "value1"}};

  std::thread first (RecorderCallback, alpha, 25, labels);     // spawn new threads that call the callback
  std::thread second (RecorderCallback, alpha, 50, labels);
  std::thread third (RecorderCallback, alpha, 25, labels1);
  std::thread fourth (NegRecorderCallback, alpha, 100, labels1); // negative values

  first.join();                
  second.join();               
  third.join();
  fourth.join();

  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[0], 0); // min
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[1], 49); // max
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[2], 1525); // sum
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels)]->get_aggregator()->get_values()[3], 75); // count

  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[0], -99); // min
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[1], 24); // max
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[2], -4650); // sum
  EXPECT_EQ(alpha->boundInstruments_[mapToString(labels1)]->get_aggregator()->get_values()[3], 125); // count
}

} //namespace sdk
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
