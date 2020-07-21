#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

OPENTELEMETRY_BEGIN_NAMESPACE

/**
 * Returns a mock span exporter meant exclusively for testing only
 */
class MockSpanExporter final : public sdk::trace::SpanExporter
{
public:
    MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received,
                     std::shared_ptr<bool> is_shutdown,
                     const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0)) noexcept
        : spans_received_(spans_received), is_shutdown_(is_shutdown), export_delay_(export_delay)
    {}

    std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
    {
        return std::unique_ptr<sdk::trace::Recordable>(new sdk::trace::SpanData);
    }

    sdk::trace::ExportResult Export(const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override
    {
        std::this_thread::sleep_for(export_delay_);

        for (auto &recordable : recordables)
        {
            auto span = std::unique_ptr<sdk::trace::SpanData>(
                static_cast<sdk::trace::SpanData *>(recordable.release()));

            if (span != nullptr)
            {
                spans_received_->push_back(std::move(span));
            }
        }

        return sdk::trace::ExportResult::kSuccess;
    }

    void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
    {
        *is_shutdown_ = true;
    }

private:
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received_;

    std::shared_ptr<bool> is_shutdown_;

    // Meant exclusively to test force flush timeout
    const std::chrono::milliseconds export_delay_;
};

/**
 * Fixture Class
 */
class BatchSpanProcessorTestPeer : public testing::Test
{
public:
    std::shared_ptr<sdk::trace::SpanProcessor> GetMockProcessor(
        std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received,
        std::shared_ptr<bool> is_shutdown,
        const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0),
        const int schedule_delay_millis = 5000,
        const int max_queue_size = 2048,
        const int max_export_batch_size = 512
    )
    {
        return std::shared_ptr<sdk::trace::SpanProcessor>(
            new sdk::trace::BatchSpanProcessor(GetMockExporter(spans_received, is_shutdown, export_delay),
                                               max_queue_size,
                                               schedule_delay_millis,
                                               max_export_batch_size));
    }


    std::unique_ptr<std::vector<std::unique_ptr<sdk::trace::Recordable>>> GetTestSpans(
        std::shared_ptr<sdk::trace::SpanProcessor> processor,
        const int num_spans
    )
    {
        std::unique_ptr<std::vector<std::unique_ptr<sdk::trace::Recordable>>> test_spans(
            new std::vector<std::unique_ptr<sdk::trace::Recordable>>
        );

        for(int i = 0; i < num_spans; ++i)
        {
            test_spans->push_back(processor->MakeRecordable());
            static_cast<sdk::trace::SpanData*>(test_spans->at(i).get())->SetName("Span " + i);
        }   

        return test_spans;
    }

private:
    std::unique_ptr<sdk::trace::SpanExporter> GetMockExporter(
        std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received,
        std::shared_ptr<bool> is_shutdown,
        const std::chrono::milliseconds export_delay = std::chrono::milliseconds(0)
    )
    {
        return std::unique_ptr<sdk::trace::SpanExporter>(new MockSpanExporter(spans_received, is_shutdown, export_delay));
    }
};

/* ##################################   TESTS   ############################################ */

TEST_F(BatchSpanProcessorTestPeer, TestShutdown)
{
    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    auto batch_processor = GetMockProcessor(spans_received, is_shutdown);
    const int num_spans = 3;

    auto test_spans = GetTestSpans(batch_processor, num_spans);

    for(int i = 0; i < num_spans; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Wait a bit to add spans to the buffer and for the background worker
    // thread of the batch processor to wait on a condition variable.
    // Helps in proper preemption of the worker thread.
    // NOTE: If this delay is not added, it's possible for Shutdown() to notify the
    //       worker thread even before it starts waiting, causing it to sleep for
    //       the entire 'schedule_delay_millis' timeout
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    batch_processor->Shutdown();

    EXPECT_EQ(num_spans, spans_received->size());
    for(int i = 0; i < num_spans; ++i)
    {
        EXPECT_EQ("Span " + i, spans_received->at(i)->GetName());
    }
   
    EXPECT_TRUE(*is_shutdown);
}

TEST_F(BatchSpanProcessorTestPeer, TestForceFlush)
{
    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    auto batch_processor = GetMockProcessor(spans_received, is_shutdown);
    const int num_spans = 3;

    auto test_spans = GetTestSpans(batch_processor, num_spans);

    for(int i = 0; i < num_spans; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Wait a bit here too for the same reasons as mentioned in the previous test (TestShutdown)
    // Helps in proper preemption of the worker thread
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // force flush
    batch_processor->ForceFlush(std::chrono::milliseconds(100));

    EXPECT_EQ(num_spans, spans_received->size());
    for(int i = 0; i < num_spans; ++i)
    {
        EXPECT_EQ("Span " + i, spans_received->at(i)->GetName());
    } 

    // Create some more spans to make sure that the processor still works
    auto more_test_spans = GetTestSpans(batch_processor, num_spans);
    for(int i = 0; i < num_spans; ++i){
        batch_processor->OnEnd(std::move(more_test_spans->at(i)));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    batch_processor->ForceFlush(std::chrono::milliseconds(100));

    EXPECT_EQ(num_spans*2, spans_received->size());
    for(int i = 0; i < num_spans; ++i)
    {
        EXPECT_EQ("Span " + i%3, spans_received->at(i)->GetName());
    } 
    // Wait a bit here so that the worker thread can go back to waiting on condition
    // variable and the shutdown() method can subsequently wake the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// TODO : Discuss and revise this function (maybe force_flush can return a bool)
TEST_F(BatchSpanProcessorTestPeer, TestForceFlushTimeout)
{
    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    const std::chrono::milliseconds export_delay(300);
    const int num_spans = 3;

    auto batch_processor = GetMockProcessor(spans_received, is_shutdown, export_delay);

    auto test_spans = GetTestSpans(batch_processor, num_spans);

    for(int i = 0; i < num_spans; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Helps in proper preemption of the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // force flush
    batch_processor->ForceFlush(std::chrono::milliseconds(100));

    // NOTE: It is also possible that some but not all spans were exported
    // TODO: Discuss this 
    EXPECT_EQ(0, spans_received->size());

    // Wait a bit here so that the worker thread can go back to waiting on condition
    // variable and the shutdown() method can subsequently wake up the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

TEST_F(BatchSpanProcessorTestPeer, TestManySpansLoss)
{
    /* Test that when exporting more than max_queue_size spans, some are most likely lost*/

    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    const int max_queue_size = 4096;

    auto batch_processor = GetMockProcessor(spans_received, is_shutdown);

    auto test_spans = GetTestSpans(batch_processor, max_queue_size);

    for(int i = 0; i < max_queue_size; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Helps in proper preemption of the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // force flush
    batch_processor->ForceFlush(std::chrono::milliseconds(100));


    EXPECT_GE(max_queue_size, spans_received->size());

    // Wait a bit here so that the worker thread can go back to waiting on condition
    // variable and the shutdown() method can subsequently wake up the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(BatchSpanProcessorTestPeer, TestManySpansLossLess)
{
    /* Test that no spans are lost when sending max_queue_size spans */

    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    const int num_spans = 2048;

    auto batch_processor = GetMockProcessor(spans_received, is_shutdown);

    auto test_spans = GetTestSpans(batch_processor, num_spans);

    for(int i = 0; i < num_spans; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Helps in proper preemption of the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // force flush
    batch_processor->ForceFlush(std::chrono::milliseconds(100));


    EXPECT_EQ(num_spans, spans_received->size());
    for(int i = 0; i < num_spans; ++i)
    {
        EXPECT_EQ("Span " + i, spans_received->at(i)->GetName());
    }

    // Wait a bit here so that the worker thread can go back to waiting on condition
    // variable and the shutdown() method can subsequently wake up the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

TEST_F(BatchSpanProcessorTestPeer, TestScheduleDelayMillis)
{
    /* Test that max_export_batch_size spans are exported every schedule_delay_millis
       seconds */

    std::shared_ptr<bool> is_shutdown(new bool(false));
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received(
        new std::vector<std::unique_ptr<sdk::trace::SpanData>>);

    const std::chrono::milliseconds export_delay(0);
    const int schedule_delay_millis = 200;
    const int max_export_batch_size = 512; // default value

    auto batch_processor = GetMockProcessor(spans_received, 
                                            is_shutdown, 
                                            export_delay,
                                            schedule_delay_millis);

    auto test_spans = GetTestSpans(batch_processor, max_export_batch_size);

    for(int i = 0; i < max_export_batch_size; ++i){
        batch_processor->OnEnd(std::move(test_spans->at(i)));
    }

    // Sleep for schedule_delay_millis milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(schedule_delay_millis + 10));

    // Spans should be exported by now
    EXPECT_EQ(max_export_batch_size, spans_received->size());
    for(int i = 0; i < max_export_batch_size; ++i)
    {
        EXPECT_EQ("Span " + i, spans_received->at(i)->GetName());
    }

    // Wait a bit here so that the worker thread can go back to waiting on condition
    // variable and the shutdown() method can subsequently wake up the worker thread
    // TODO: Discuss this
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

OPENTELEMETRY_END_NAMESPACE