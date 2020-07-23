#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include <thread>
#include <condition_variable>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

/* Forward declaration of the Circular Buffer to avoid pulling in external dependencies in //sdk:headers */
namespace common {
    template <class T>
    class CircularBuffer;
}

namespace trace
{   

/**
 * This is an implementation of the SpanProcessor which creates batches of finished spans and passes 
 * the export-friendly span data representations to the configured SpanExporter.
 */
class BatchSpanProcessor : public SpanProcessor 
{
public:
    /**
     * Creates a batch span processor by configuring the specified exporter and other parameters
     * as per the official, language-agnostic opentelemetry specs.
     * 
     * @param exporter - The backend exporter to pass the ended spans to
     * @param max_queue_size -  The maximum buffer/queue size. After the size is reached, spans are dropped.
     * @param schedule_delay_millis - The time interval between two consecutive exports.
     * @param max_export_batch_size - The maximum batch size of every export. It must be smaller or equal to max_queue_size
     */
    explicit BatchSpanProcessor(std::unique_ptr<SpanExporter>&& exporter,
                                const size_t max_queue_size = 2048,
                                const std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000),
                                const size_t max_export_batch_size = 512);

    /**
     * Requests a Recordable(Span) from the configured exporter.
     * 
     * @return A recordable generated by the backend exporter
     */
    std::unique_ptr<Recordable> MakeRecordable() noexcept override;

    /**
     * Called when a span is started.
     * NOTE: This method is a no-op.
     *  
     * @param span - The span that just started
     */
    void OnStart(Recordable &span) noexcept override;

    /**
     * Called when a span ends.
     * 
     * @param span - A recordable for a span that just ended
     */
    void OnEnd(std::unique_ptr<Recordable> &&span) noexcept override;

    /**
     * Export all ended spans that have not been exported yet.
     * 
     * @param timeout - An optional timeout. A default timeout of 0 means no timeout.
     */
    void ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

    /**
     * Shuts down the processor and does any cleanup required. Completely drains the buffer/queue of all its
     * ended spans and passes them to the exporter. Any subsequent calls to OnStart, OnEnd, ForceFlush or
     * Shutdown will return immediately without doing anything.
     * 
     * @param timeout - An optional timeout. A default timeout of 0 means no timeout.
     */
    void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

    /**
     * Class destructor which invokes the Shutdown() method. The Shutdown() method is supposed to be invoked
     * when the Tracer is shutdown (as per other languages), but the C++ Tracer only takes shared ownership of the processor, 
     * and thus doesn't call Shutdown (as the processor might be shared with other Tracers).
     */
    ~BatchSpanProcessor();

private:
    /**
     * The background routine performed by the worker thread.
     */
    void DoBackgroundWork();

    /**
     * Exports all ended spans to the configured exporter.
     * 
     * @param buffer - The buffer with ended spans to export
     * @param was_force_flush_called - A flag to check if the current export is the result
     *                                 of a call to ForceFlush method. If true, then we have to
     *                                 notify the main thread to wake it up in the ForceFlush 
     *                                 method.
     */
    void Export(std::unique_ptr<common::CircularBuffer<Recordable>>& buffer,
                const bool was_for_flush_called);

    /**
     * Called when Shutdown() is invoked. Completely drains the queue of all its ended spans and passes them 
     * to the exporter.
     */
    void DrainQueue();

    /**
     * Consumes and copies the appropriate amount of spans from the buffer_ 
     * to a copy buffer which is then exported. This helps unblock all producers
     * and increases the overall synchronization performance.
     * 
     * @param was_force_flush_called - A flag to check if the current export is the result
     *                                 of a call to ForceFlush method. If true, we consume 
     *                                 and copy the entire buffer_ to the copy buffer. Otherwise,
     *                                 we calculate the appropriate size to export.
     * @return A unique pointer to the copy buffer
     */
    std::unique_ptr<common::CircularBuffer<Recordable>> CopySpans(const bool was_force_flush_called);

    /* The configured backend exporter */
    std::unique_ptr<SpanExporter> exporter_;

    /* The background worker thread */
    std::unique_ptr<std::thread> worker_thread_;

    /* Configurable parameters as per the official specs */
    const std::chrono::milliseconds schedule_delay_millis_;
    const size_t max_queue_size_;
    const size_t max_export_batch_size_;

    /* Synchronization primitives */
    std::condition_variable cv_, force_flush_cv_;
    std::mutex cv_m_, force_flush_cv_m_;

    /* The buffer/queue to which the ended spans are added */
    std::unique_ptr<common::CircularBuffer<Recordable>> buffer_;

    /* Important boolean flags to handle the workflow of the processor */
    bool is_shutdown_{false}, is_force_flush_{false}, is_force_flush_notified_{false};
};   

} // trace
} // sdk
OPENTELEMETRY_END_NAMESPACE


