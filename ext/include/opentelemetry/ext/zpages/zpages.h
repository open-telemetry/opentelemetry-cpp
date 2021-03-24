#pragma once

#include <chrono>
#include <memory>

#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/ext/zpages/tracez_http_server.h"
#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/ext/zpages/tracez_shared_data.h"

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

using opentelemetry::ext::zpages::TracezDataAggregator;
using opentelemetry::ext::zpages::TracezHttpServer;
using opentelemetry::ext::zpages::TracezSpanProcessor;
using opentelemetry::ext::zpages::TracezSharedData;
using std::chrono::microseconds;

/**
 * Wrapper for zPages that initializes all the components required for zPages,
 * and starts the HTTP server in the constructor and ends it in the destructor.
 * The constructor and destructor for this object is private to prevent
 * creation other than by calling the static function Initialize(). This follows the
 * meyers singleton pattern and only a single instance of the class is allowed.
 */
class ZPages
{
public:
  /**
   * This function is called if the user wishes to include zPages in their
   * application. It creates a static instance of this class.
   */
  static void Initialize() { static ZPages instance; }

private:
  /**
   * Constructor is responsible for initializing the tracer, tracez processor,
   * tracez data aggregator and the tracez server. The server is also started in
   * constructor.
   */
  ZPages()
  {
    auto tracez_shared_data = std::make_shared<TracezSharedData>();
    auto tracez_processor_ = std::make_shared<TracezSpanProcessor>(tracez_shared_data);
    auto tracez_provider_  = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
        new opentelemetry::sdk::trace::TracerProvider(tracez_processor_));

    auto tracez_aggregator =
        std::unique_ptr<TracezDataAggregator>(new TracezDataAggregator(tracez_shared_data));

    tracez_server_ =
        std::unique_ptr<TracezHttpServer>(new TracezHttpServer(std::move(tracez_aggregator)));

    tracez_server_->start();

    opentelemetry::trace::Provider::SetTracerProvider(tracez_provider_);
  }

  ~ZPages()
  {
    // shut down the server when the object goes out of scope(at the end of the
    // program)
    tracez_server_->stop();
  }
  std::unique_ptr<TracezHttpServer> tracez_server_;
};
