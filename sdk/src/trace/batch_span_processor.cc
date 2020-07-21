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
    buffer_ = std::unique_ptr<CircularBuffer<Recordable>>(new CircularBuffer<Recordable>(max_queue_size_));
    
    //Start the background worker thread
    worker_thread_ = std::unique_ptr<std::thread>(new std::thread(&BatchSpanProcessor::DoBackgroundWork, this));

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
    if(is_shutdown_){
        // TODO: log in glog
        return;
    }

    if(static_cast<int>(buffer_->size()) == max_queue_size_){
        // TODO: glog that spans will likely be dropped
    }

    buffer_->Add(span);

    // If the queue gets at least half full a preemptive notification is sent to the worker thread to
    // start a new export cycle.
    if(static_cast<int>(buffer_->size()) >= max_queue_size_ / 2){
        // signal the worker thread
        cv_.notify_one();
    }
}

void BatchSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept 
{    
    if(is_shutdown_ == true){
        // TODO: glog that processor is already shutdown
    }

    is_force_flush_ = true;
    buffer_->Add(FORCE_FLUSH_TOKEN);

    // wake up the worker thread
    cv_.notify_one();

    // wait on flush token span to be processed
    std::unique_lock<std::mutex> force_flush_lk(force_flush_cv_m_);
    auto status = force_flush_cv_.wait_for(force_flush_lk, timeout);

    is_force_flush_ = false;

    if(status == std::cv_status::timeout){
        // TODO: glog timeout
    } else {
        // TODO: glog no timeout
    }
}

void BatchSpanProcessor::DoBackgroundWork(){
    int timeout = schedule_delay_millis_;

    while (true)
    {
        // We wait for the buffer to fill up to max_export_batch_size 
        //                     OR
        // We see if the force_flush() method has been invoked
        if(static_cast<int>(buffer_->size()) < max_export_batch_size_ && 
           is_force_flush_ == false)
           {
            std::unique_lock<std::mutex> lk(cv_m_);
            cv_.wait_for(lk, std::chrono::milliseconds(timeout));

            // If shutdown has been invoked, break out of the loop
            // and drain out the queue.
            if(is_shutdown_) break;

            // In case of spurious wake up, we export only if we have atleast one span in the batch.
            // This is acceptable because batching is a best mechanism effort here.
            if(static_cast<int>(buffer_->size()) == 0) continue;
        }

        auto start = std::chrono::steady_clock::now();
        Export();
        auto end = std::chrono::steady_clock::now(); 
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        timeout = schedule_delay_millis_ - duration.count();
    }

    // Drain the queue in case there are any spans still left. 
    // Invoked when shutdown() is called.
    DrainQueue();
}

void BatchSpanProcessor::Export(){
    // Get the appropriate size
    const int buffer_size = static_cast<int>(buffer_->size());
    const int num_spans_to_export = buffer_size >= max_export_batch_size_ ? max_export_batch_size_ : buffer_size;

    std::vector<std::unique_ptr<Recordable>> spans_arr;
    
    bool is_force_flush_token_present = false;

    buffer_->Consume(
    num_spans_to_export, [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
        std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
        ptr.Swap(swap_ptr);

        // Check if we just processed/consumed the force flush token
        if(swap_ptr == nullptr) is_force_flush_token_present = true;

        else spans_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
        return true;
        });
    });

    exporter_->Export(nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()));

    // If we processed the force flush token, notify
    if (is_force_flush_token_present) 
    {
        force_flush_cv_.notify_one();
    }
    
}

void BatchSpanProcessor::DrainQueue(){
    while(static_cast<int>(buffer_->size())) Export();
}

void BatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept 
{
    is_shutdown_ = true;

    // Notify thread, drain the queue and wait for the thread
    cv_.notify_one();
    worker_thread_->join();
    worker_thread_.reset();

    exporter_->Shutdown(timeout);
}

BatchSpanProcessor::~BatchSpanProcessor(){
    if(is_shutdown_ == false) Shutdown();
}

} // trace
} // sdk
OPENTELEMETRY_END_NAMESPACE


