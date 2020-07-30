#pragma once

#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/exporters/ostream/metrics_exporter.h"
#include "opentelemetry/version.h"

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace metrics_api = opentelemetry::metrics;
namespace trace_api   = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class Processor {
public:
    Processor() = default;
    
    void process(std::vector<Record> alpha){}
    
    std::vector<Record> CheckpointSelf() {
        return std::vector<Record>();
    }
};

class PushController
{

public:
  PushController(nostd::shared_ptr<metrics_api::Meter> meter,
                 nostd::unique_ptr<MetricsExporter> exporter,
                 nostd::shared_ptr<Processor> processor,
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
   * Used to check if the metrics pipeline is currecntly active
   *
   * @param none
   * @return true when active, false when on standby
   */
  bool isActive() { return active_; }

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
    if (!active_)
    {
      active_ = true;
      std::thread runner(&PushController::run, this);
      runner.detach();
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
    active_ = false;
    usleep(period_);
    tick();  // flush metrics sitting in the processor
  }

  /*
   * Run the tick function at a regular interval. This function
   * should be run in its own thread.
   *
   * Used to wait between collection intervals.
   */
  void run()
  {
    while (active_)
    {
      tick();
      usleep(period_);
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
    std::cout << "tick" << std::endl;
    processor_->process(dynamic_cast<Meter*>(meter_.get())->Collect());
    std::vector<Record> checkpointed = processor_->CheckpointSelf();
    exporter_->Export(checkpointed);
  }

  nostd::shared_ptr<metrics_api::Meter> meter_;
  nostd::unique_ptr<MetricsExporter> exporter_;
  nostd::shared_ptr<Processor> processor_;
  bool active_ = false;
  unsigned int period_;
  unsigned int timeout_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
