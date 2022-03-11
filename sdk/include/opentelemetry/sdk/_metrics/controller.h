// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <atomic>
#  include <iostream>
#  include <sstream>
#  include <thread>
#  include <vector>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/sdk/_metrics/exporter.h"
#  include "opentelemetry/sdk/_metrics/meter.h"
#  include "opentelemetry/sdk/_metrics/processor.h"
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class PushController
{

public:
  PushController(nostd::shared_ptr<opentelemetry::metrics::Meter> meter,
                 nostd::unique_ptr<MetricsExporter> exporter,
                 nostd::shared_ptr<MetricsProcessor> processor,
                 double period,
                 int timeout = 30)
  {
    meter_     = meter;
    exporter_  = std::move(exporter);
    processor_ = processor;
    timeout_   = (unsigned int)(timeout * 1000000);  // convert seconds to microseconds
    period_    = (unsigned int)(period * 1000000);
  }

  /*
   * Used to check if the metrics pipeline is currently active
   *
   * @param none
   * @return true when active, false when on standby.  This is a best guess estimate
   * and the boolean from start() should be used to determine wheher the pipeline
   * was initiated successfully.
   */
  bool isActive() { return active_.load(); }

  /*
   * Begins the data processing and export pipeline.  The function first ensures that the pipeline
   * is not already running.  If not, it begins and detaches a new thread for the Controller's run
   * function which periodically polls the instruments for their data.
   *
   * @param none
   * @return a boolean which is true when the pipeline is successfully started and false when
   * already active
   */
  bool start()
  {
    if (!active_.exchange(true))
    {
      runner_ = std::thread(&PushController::run, this);
      return true;
    }
    return false;
  }

  /*
   * Ends the processing and export pipeline then exports metrics one last time
   * before returning.
   *
   * @param none
   * @return none
   */
  void stop()
  {
    if (active_.exchange(false))
    {
      if (runner_.joinable())
      {
        runner_.join();
      }
      tick();  // flush metrics sitting in the processor
    }
  }

private:
  /*
   * Run the tick function at a regular interval. This function
   * should be run in its own thread.
   *
   * Used to wait between collection intervals.
   */
  void run()
  {
    if (!running_.exchange(true))
    {
      while (active_.load())
      {
        tick();
        std::this_thread::sleep_for(std::chrono::microseconds(period_));
      }
      running_.exchange(false);
    }
  }

  /*
   * Tick
   *
   * Called at regular intervals, this function collects all values from the
   * member variable meter_, then sends them to the processor_ for
   * processing. After the records have been processed they are sent to the
   * exporter_ to be exported.
   *
   */
  void tick()
  {
    this->mu_.lock();
    std::vector<Record> collected = dynamic_cast<Meter *>(meter_.get())->Collect();
    for (const auto &rec : collected)
    {
      processor_->process(rec);
    }
    collected = processor_->CheckpointSelf();
    processor_->FinishedCollection();
    exporter_->Export(collected);
    this->mu_.unlock();
  }

  nostd::shared_ptr<opentelemetry::metrics::Meter> meter_;
  nostd::unique_ptr<MetricsExporter> exporter_;
  nostd::shared_ptr<MetricsProcessor> processor_;
  std::thread runner_;
  std::mutex mu_;
  std::atomic<bool> active_  = ATOMIC_VAR_INIT(false);
  std::atomic<bool> running_ = ATOMIC_VAR_INIT(false);
  unsigned int period_;
  unsigned int timeout_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
