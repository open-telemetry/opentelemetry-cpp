#pragma once

#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "src/common/circular_buffer.h"

#include <vector>

using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;


OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{   

BatchSpanProcessor::BatchSpanProcessor(std::unique_ptr<SpanExporter>&& exporter,
                                       const int max_queue_size,
                                       const int schedule_delay_millis,
                                       const int max_export_batch_size)
       :exporter_(std::move(exporter)),
        max_queue_size_(max_queue_size), 
        schedule_delay_millis_(schedule_delay_millis),
        max_export_batch_size_(max_export_batch_size)   
{  
    buffer_ = std::unique_ptr<CircularBuffer<Recordable>>(
                    new CircularBuffer<Recordable>(max_queue_size_));
    
    //Start the background worker thread
    worker_thread_ = std::unique_ptr<std::thread>(
                        new std::thread(&BatchSpanProcessor::DoBackgroundWork, this));
} 

std::unique_ptr<Recordable> BatchSpanProcessor::MakeRecordable() noexcept 
{
    return exporter_->MakeRecordable();
}

void BatchSpanProcessor::OnStart(Recordable &span) noexcept  
{
    // no-op
}

void BatchSpanProcessor::OnEnd(std::unique_ptr<Recordable> &&span) noexcept 
{
    if(is_shutdown_ == true){
        // TODO: log in glog
        return;
    }

    std::unique_lock<std::mutex> lk(cv_m_);

    if(static_cast<int>(buffer_->size()) >= max_queue_size_){
        // TODO: glog that spans will likely be dropped
    }

    buffer_->Add(span);

    // If the queue gets at least half full a preemptive notification is 
    // sent to the worker thread to start a new export cycle.
    if(static_cast<int>(buffer_->size()) >= max_queue_size_ / 2){
        // signal the worker thread
        cv_.notify_one();
    }

    lk.unlock();
}

void BatchSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept 
{    
    if(is_shutdown_ == true){
        // TODO: glog that processor is already shutdown
    }

    is_force_flush_ = true;

    std::chrono::microseconds timeout_left = timeout;

    // Keep attempting to wake up the worker thread and monitor timeout left
    // accordingly.
    while(is_force_flush_ == true && timeout_left.count() > 0) {
        auto start = std::chrono::steady_clock::now();
        cv_.notify_one();
        std::this_thread::sleep_for(std::chrono::nanoseconds(50));
        auto end = std::chrono::steady_clock::now();
        timeout_left -= std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    }

    // Now wait for the worker thread to signal back from the Export method
    //                      OR
    // Wait until we timeout
    std::unique_lock<std::mutex> lk(force_flush_cv_m_);
    while(timeout_left.count() > 0 && is_force_flush_notified_ == false)
    {
        auto start_time = std::chrono::steady_clock::now();
        force_flush_cv_.wait_for(lk, timeout_left);
        timeout_left -= std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now() - start_time);
    }
        
    lk.unlock();

    // This flag handles the case when we have a timeout too because the notification
    // loop (lines 202 - 205) in the Export method keeps looping until is_force_flush_notified_ 
    // holds true. As soon as this flag is turned to false, the notification loop breaks.
    is_force_flush_notified_ = false;

    if(timeout_left.count() <= 0)
    {
        // TODO: glog timeout
    }
    else
    {
        // TODO: glog no timeout
    }
}

void BatchSpanProcessor::DoBackgroundWork(){
    int timeout = schedule_delay_millis_;

    while (true)
    {
        std::unique_lock<std::mutex> lk(cv_m_);

        // If we already have max_export_batch_size_ spans in the buffer, better to export them
        // now
        if (static_cast<int>(buffer_->size()) < max_export_batch_size_)
        {
            // In case of spurious wake up, we export only if we have atleast one span
            // in the batch. This is acceptable because batching is a best mechanism 
            // effort here.
            do 
            {
                cv_.wait_for(lk, std::chrono::milliseconds(timeout));

                // If shutdown has been invoked, break out of the loop
                // and drain out the queue.
                if(is_shutdown_ == true)
                {
                    is_shutdown_ = false;
                    DrainQueue();
                    return;
                } 

                if(is_force_flush_ == true) break;                
            } 
            while(buffer_->empty() == true);
        }

        // Get the value of is_force_flush_ to check whether this export is 
        // a result of a ForceFlush call. This flag is propagated to the Export
        // method as well.
        bool was_force_flush_called = is_force_flush_;

        // Based on whether or not ForceFlush was invoked, consume and copy
        // the right amount of spans from the buffer_ into another buffer.
        auto buffer_copy = CopySpans(was_force_flush_called);

        // Set the flag back to false to notify main thread in case ForceFlush
        // was invoked.
        if(is_force_flush_ == true) is_force_flush_ = false;

        // Unlock here to unblock all producers
        lk.unlock();

        auto start = std::chrono::steady_clock::now();
        Export(buffer_copy, was_force_flush_called);
        auto end = std::chrono::steady_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        timeout = schedule_delay_millis_ - duration.count();
    }
}

void BatchSpanProcessor::Export(std::unique_ptr<common::CircularBuffer<Recordable>>& buffer,
                                const bool was_force_flush_called)
{
    std::vector<std::unique_ptr<Recordable>> spans_arr;
    
    buffer->Consume(
    buffer->size(), [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
        std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
        ptr.Swap(swap_ptr);
        spans_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
        return true;
        });
    });

    exporter_->Export(nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()));    

    if(is_shutdown_ == true) return;

    // Notify the main thread in case this export was the result of a force flush.
    if(was_force_flush_called == true)
    {
        is_force_flush_ = false;
        is_force_flush_notified_ = true;

        while(is_force_flush_notified_ == true)
        {
            force_flush_cv_.notify_one();
        }
    }
}

std::unique_ptr<common::CircularBuffer<Recordable>> BatchSpanProcessor::CopySpans(
    const bool was_force_flush_called
)
{
    // Make a copy of the buffer to avoid blocking other producers.
    std::unique_ptr<common::CircularBuffer<Recordable>> buffer_copy(
        new common::CircularBuffer<Recordable>(max_queue_size_)
    );

    // Get the appropriate size.
    const int buffer_size = static_cast<int>(buffer_->size());

    if(was_force_flush_called == true)
    {
        buffer_.swap(buffer_copy);
    }
    else
    {
        const int num_spans_to_export = buffer_size >= max_export_batch_size_ ? 
                                                max_export_batch_size_ : buffer_size;

        buffer_->Consume(
            num_spans_to_export, [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
                range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
                std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
                ptr.Swap(swap_ptr);
                buffer_copy->Add(swap_ptr);
                return true;
                });
            });
    }

    return buffer_copy;
}

void BatchSpanProcessor::DrainQueue()
{
    while(static_cast<int>(buffer_->size()) > 0) Export(buffer_, false);
}

void BatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept 
{
    auto start_time = std::chrono::steady_clock::now();

    is_shutdown_ = true;

    // Notify thread, drain the queue and wait for the thread.
    while(is_shutdown_ == true) cv_.notify_one();

    is_shutdown_ = true;
    worker_thread_->join();
    worker_thread_.reset();
    
    auto timeout_left = timeout - std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::steady_clock::now() - start_time);    
    exporter_->Shutdown(timeout_left);
}

BatchSpanProcessor::~BatchSpanProcessor(){
    if(is_shutdown_ == false) Shutdown();
}

} // trace
} // sdk
OPENTELEMETRY_END_NAMESPACE


