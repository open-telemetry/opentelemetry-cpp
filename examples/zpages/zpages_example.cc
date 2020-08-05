/**
 * This is a basic example for zpages that helps users get familiar with how to
 * use this feature in OpenTelemetery
 */
#include <iostream>
#include <string>
#include <chrono>

#include "opentelemetry/ext/zpages/zpages.h" // Required file include for zpages

int main(int argc, char* argv[]) {
  
  /** 
   * The following line initializes zPages and starts a webserver at 
   * http://localhost:30000/tracez/ where spans that are created in the application
   * can be viewed.
   * Note that the webserver is destroyed after the application ends execution. 
   */
  zPages::Initialize();
  auto tracer = opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("");
  
  std::cout << "This example for zPages creates a few types of spans and then "
            << "creates a span every second for the duration of the application"
            << "\n";
  // Create a span of each type(running, completed and error)
  auto running_span = tracer->StartSpan("examplespan1");
  running_span->SetAttribute("attribute1", 314159);
  running_span->End();
  
  tracer->StartSpan("examplespan")->End();
  tracer->StartSpan("examplespan")->SetStatus(
      opentelemetry::trace::CanonicalCode::CANCELLED, "Cancelled example");
  
  // Create another running span with a different name
  auto running_span2 = tracer->StartSpan("examplespan2");
  // Create a completed span every second till user stops the loop
  std::cout << "Presss CTRL+C to stop...\n";
  while(true){
    std::this_thread::sleep_for(seconds(1));
    tracer->StartSpan("examplespan")->End();
  }
}
