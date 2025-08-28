// Example demonstrating export_unsampled_spans feature
// This example shows how to configure span processors to export unsampled spans

#include <iostream>
#include <memory>

#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/simple_processor_options.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_flags.h"

namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_api = opentelemetry::trace;

// Simple mock exporter for demonstration
class MockExporter : public trace_sdk::SpanExporter {
public:
  explicit MockExporter(const std::string& name) : name_(name) {}
  
  std::unique_ptr<trace_sdk::Recordable> MakeRecordable() noexcept override {
    return std::make_unique<trace_sdk::SpanData>();
  }
  
  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<trace_sdk::Recordable>>& recordables) noexcept override {
    std::cout << name_ << " exported " << recordables.size() << " spans" << std::endl;
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  
  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
  
private:
  std::string name_;
};

// Create a test span with specific sampling status
std::unique_ptr<trace_sdk::SpanData> CreateTestSpan(bool sampled) {
  auto span = std::make_unique<trace_sdk::SpanData>();
  span->SetName("test_span");
  
  // Set up valid context with proper sampling
  trace_api::TraceFlags flags(sampled ? trace_api::TraceFlags::kIsSampled : 0);
  uint8_t trace_id_bytes[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  uint8_t span_id_bytes[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  
  trace_api::TraceId trace_id{opentelemetry::nostd::span<const uint8_t, 16>(trace_id_bytes)};
  trace_api::SpanId span_id{opentelemetry::nostd::span<const uint8_t, 8>(span_id_bytes)};
  
  trace_api::SpanContext context(trace_id, span_id, flags, false);
  span->SetIdentity(context, trace_api::SpanId());
  
  return span;
}

int main() {
  std::cout << "OpenTelemetry C++ Export Unsampled Spans Demo\n" << std::endl;
  
  // Example 1: BatchSpanProcessor without export_unsampled_spans (default behavior)
  {
    std::cout << "=== Example 1: BatchSpanProcessor (default behavior) ===" << std::endl;
    
    trace_sdk::BatchSpanProcessorOptions options;
    // export_unsampled_spans is false by default
    
    auto processor = std::make_unique<trace_sdk::BatchSpanProcessor>(
        std::make_unique<MockExporter>("BatchProcessor-Default"), options);
    
    // Create one sampled and one unsampled span
    auto sampled_span = CreateTestSpan(true);
    auto unsampled_span = CreateTestSpan(false);
    
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(sampled_span.release()));
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(unsampled_span.release()));
    
    processor->ForceFlush();
    std::cout << "Expected: Only 1 span exported (sampled span only)\n" << std::endl;
  }
  
  // Example 2: BatchSpanProcessor with export_unsampled_spans enabled
  {
    std::cout << "=== Example 2: BatchSpanProcessor (export_unsampled_spans = true) ===" << std::endl;
    
    trace_sdk::BatchSpanProcessorOptions options;
    options.export_unsampled_spans = true;  // Enable exporting unsampled spans
    
    auto processor = std::make_unique<trace_sdk::BatchSpanProcessor>(
        std::make_unique<MockExporter>("BatchProcessor-WithUnsampled"), options);
    
    // Create one sampled and one unsampled span
    auto sampled_span = CreateTestSpan(true);
    auto unsampled_span = CreateTestSpan(false);
    
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(sampled_span.release()));
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(unsampled_span.release()));
    
    processor->ForceFlush();
    std::cout << "Expected: 2 spans exported (both sampled and unsampled)\n" << std::endl;
  }
  
  // Example 3: SimpleSpanProcessor with export_unsampled_spans enabled
  {
    std::cout << "=== Example 3: SimpleSpanProcessor (export_unsampled_spans = true) ===" << std::endl;
    
    trace_sdk::SimpleSpanProcessorOptions options;
    options.export_unsampled_spans = true;  // Enable exporting unsampled spans
    
    auto processor = std::make_unique<trace_sdk::SimpleSpanProcessor>(
        std::make_unique<MockExporter>("SimpleProcessor-WithUnsampled"), options);
    
    // Create one sampled and one unsampled span
    auto sampled_span = CreateTestSpan(true);
    auto unsampled_span = CreateTestSpan(false);
    
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(sampled_span.release()));
    processor->OnEnd(std::unique_ptr<trace_sdk::Recordable>(unsampled_span.release()));
    
    std::cout << "Expected: 2 separate exports (one for each span)\n" << std::endl;
  }
  
  std::cout << "Demo completed!" << std::endl;
  return 0;
}