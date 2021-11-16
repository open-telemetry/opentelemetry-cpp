// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/**
 * This is a basic example for zpages that helps users get familiar with how to
 * use this feature in OpenTelemetery
 */
#include <chrono>
#include <iostream>
#include <string>

#include "opentelemetry/ext/zpages/zpages.h"  // Required file include for zpages

using opentelemetry::common::SteadyTimestamp;
namespace trace_api = opentelemetry::trace;

int main(int argc, char *argv[])
{

  /**
   * The following line initializes zPages and starts a webserver at
   * http://localhost:30000/tracez/ where spans that are created in the application
   * can be viewed.
   * Note that the webserver is destroyed after the application ends execution.
   */
  ZPages::Initialize();
  auto tracer = trace_api::Provider::GetTracerProvider()->GetTracer("");

  std::cout << "This example for zPages creates a few types of spans and then "
            << "creates a span every second for the duration of the application"
            << "\n";

  // Error span
  std::map<std::string, opentelemetry::common::AttributeValue> attribute_map;
  attribute_map["completed_search_for"] = "Unknown user";
  tracer->StartSpan("find user", attribute_map)
      ->SetStatus(trace_api::StatusCode::kError, "User not found");

  // Long time duration span
  std::map<std::string, opentelemetry::common::AttributeValue> attribute_map2;
  attribute_map2["completed_search_for"] = "John Doe";
  trace_api::StartSpanOptions start;
  start.start_steady_time = SteadyTimestamp(nanoseconds(1));
  trace_api::EndSpanOptions end;
  end.end_steady_time = SteadyTimestamp(nanoseconds(1000000000000));
  tracer->StartSpan("find user", attribute_map2, start)->End(end);

  // Running(deadlock) span
  std::map<std::string, opentelemetry::common::AttributeValue> attribute_map3;
  attribute_map3["searching_for"] = "Deleted user";
  auto running_span               = tracer->StartSpan("find user", attribute_map3);

  // Create a completed span every second till user stops the loop
  std::cout << "Presss CTRL+C to stop...\n";
  while (true)
  {
    std::this_thread::sleep_for(seconds(1));
    tracer->StartSpan("ping user")->End();
  }
}
