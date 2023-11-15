// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"

#if defined(HAVE_GSL)
#  include <gsl/gsl>
#else
#  include <assert.h>
#endif

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

#ifdef ENABLE_ASYNC_EXPORT

namespace
{
struct OPENTELEMETRY_LOCAL_SYMBOL OtlpGrpcAsyncCallDataBase
{
  using GrpcAsyncCallback = bool (*)(OtlpGrpcAsyncCallDataBase *);

  std::unique_ptr<google::protobuf::Arena> arena;
  grpc::Status grpc_status;
  std::unique_ptr<grpc::ClientContext> grpc_context;

  opentelemetry::sdk::common::ExportResult export_result =
      opentelemetry::sdk::common::ExportResult::kFailure;
  GrpcAsyncCallback grpc_async_callback = nullptr;

  OtlpGrpcAsyncCallDataBase() {}
  virtual ~OtlpGrpcAsyncCallDataBase() {}
};

template <class GrpcRequestType, class GrpcResponseType>
struct OPENTELEMETRY_LOCAL_SYMBOL OtlpGrpcAsyncCallData : public OtlpGrpcAsyncCallDataBase
{
  using RequestType  = GrpcRequestType;
  using ResponseType = GrpcResponseType;

  RequestType *request   = nullptr;
  ResponseType *response = nullptr;
  std::unique_ptr<grpc::ClientAsyncResponseReaderInterface<ResponseType>> response_reader;

  std::function<bool(opentelemetry::sdk::common::ExportResult,
                     std::unique_ptr<google::protobuf::Arena> &&,
                     const RequestType &,
                     ResponseType *)>
      result_callback;

  OtlpGrpcAsyncCallData() {}
  virtual ~OtlpGrpcAsyncCallData() {}
};
}  // namespace

struct OtlpGrpcClientAsyncData
{
  std::chrono::system_clock::duration export_timeout = std::chrono::seconds{10};

  // The best performance trade-off of gRPC is having numcpu's threads and one completion queue
  // per thread, but this exporter should not cost a lot resource and we don't want to create
  // too many threads in the process. So we use one completion queue.
  grpc::CompletionQueue cq;

  // Running requests, this is used to limit the number of concurrent requests.
  std::atomic<std::size_t> running_requests{0};
  // Request counter is used to record ForceFlush.
  std::atomic<std::size_t> start_request_counter{0};
  std::atomic<std::size_t> finished_request_counter{0};
  std::size_t max_concurrent_requests = 64;

  // Condition variable and mutex to control the concurrency count of running requests.
  std::mutex session_waker_lock;
  std::condition_variable session_waker;

  std::mutex background_thread_m;
  std::unique_ptr<std::thread> background_thread;

  // Do not use OtlpGrpcClientAsyncData() = default; here, some versions of GCC&Clang have BUGs
  // and may not initialize the member correctly. See also
  // https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
  OtlpGrpcClientAsyncData() {}
};
#endif

namespace
{
// ----------------------------- Helper functions ------------------------------
static std::string GetFileContents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents;
  contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
  finstream.close();
  return contents;
}

// If the file path is non-empty, returns the contents of the file. Otherwise returns contents.
static std::string GetFileContentsOrInMemoryContents(const std::string &file_path,
                                                     const std::string &contents)
{
  if (!file_path.empty())
  {
    return GetFileContents(file_path.c_str());
  }
  return contents;
}

static void MaybeSpawnBackgroundThread(std::shared_ptr<OtlpGrpcClientAsyncData> async_data)
{
  std::lock_guard<std::mutex> lock_guard{async_data->background_thread_m};
  if (async_data->background_thread)
  {
    return;
  }

  async_data->background_thread.reset(new std::thread([async_data]() {
    bool running = true;
    while (running)
    {
      void *tag = nullptr;
      bool ok   = false;
      // If there is no job in exporting_timeout+5 minutes, we can exit this thread and start
      // another thread later when new datas arrived.
      auto got_status = async_data->cq.AsyncNext(
          &tag, &ok,
          std::chrono::system_clock::now() + std::chrono::minutes{5} + async_data->export_timeout);
      if (grpc::CompletionQueue::SHUTDOWN == got_status)
      {
        std::lock_guard<std::mutex> internal_lock_guard{async_data->background_thread_m};
        if (async_data->background_thread)
        {
          async_data->background_thread->detach();
          async_data->background_thread.reset();
        }
        break;
      }
      if (grpc::CompletionQueue::TIMEOUT == got_status)
      {
        std::lock_guard<std::mutex> internal_lock_guard{async_data->background_thread_m};
        running = async_data->running_requests.load(std::memory_order_acquire) > 0;

        if (!running)
        {
          if (async_data->background_thread)
          {
            async_data->background_thread->detach();
            async_data->background_thread.reset();
          }
          break;
        }
      }

      if (nullptr == tag)
      {
        continue;
      }

      auto callback_data = reinterpret_cast<OtlpGrpcAsyncCallDataBase *>(tag);
      --async_data->running_requests;
      ++async_data->finished_request_counter;

      if (callback_data->grpc_status.ok())
      {
        callback_data->export_result = opentelemetry::sdk::common::ExportResult::kSuccess;
      }

      if (callback_data->grpc_async_callback)
      {
        callback_data->grpc_async_callback(callback_data);
        delete callback_data;
      }

      // Maybe wake up blocking DelegateAsyncExport() call
      async_data->session_waker.notify_all();
    }
  }));
}

template <class StubType, class RequestType, class ResponseType>
static sdk::common::ExportResult InternalDelegateAsyncExport(
    std::shared_ptr<OtlpGrpcClientAsyncData> async_data,
    StubType *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> &&arena,
    RequestType &&request,
    std::function<bool(opentelemetry::sdk::common::ExportResult,
                       std::unique_ptr<google::protobuf::Arena> &&,
                       const RequestType &,
                       ResponseType *)> &&result_callback,
    int32_t export_data_count,
    const char *export_data_name) noexcept
{
  if (async_data->running_requests.load(std::memory_order_acquire) >=
      async_data->max_concurrent_requests)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << export_data_count << " " << export_data_name
                            << " failed, exporter queue is full");
    if (result_callback)
    {
      result_callback(opentelemetry::sdk::common::ExportResult::kFailureFull, std::move(arena),
                      request, nullptr);
    }
    return opentelemetry::sdk::common::ExportResult::kFailureFull;
  }

  OtlpGrpcAsyncCallData<RequestType, ResponseType> *call_data =
      new OtlpGrpcAsyncCallData<RequestType, ResponseType>();
  call_data->arena.swap(arena);
  call_data->result_callback.swap(result_callback);

  call_data->request =
      google::protobuf::Arena::Create<RequestType>(call_data->arena.get(), std::move(request));
  call_data->response = google::protobuf::Arena::Create<ResponseType>(call_data->arena.get());

  if (call_data->request == nullptr || call_data->response == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << export_data_count << " " << export_data_name
                            << " failed, create gRPC request/response failed");

    if (call_data->result_callback)
    {
      call_data->result_callback(
          opentelemetry::sdk::common::ExportResult::kFailure, std::move(call_data->arena),
          nullptr == call_data->request ? request : *call_data->request, call_data->response);
    }

    delete call_data;
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }
  call_data->grpc_context.swap(context);

  call_data->grpc_async_callback = [](OtlpGrpcAsyncCallDataBase *base_call_data) {
    OtlpGrpcAsyncCallData<RequestType, ResponseType> *real_call_data =
        static_cast<OtlpGrpcAsyncCallData<RequestType, ResponseType> *>(base_call_data);
    if (real_call_data->result_callback)
    {
      return real_call_data->result_callback(real_call_data->export_result,
                                             std::move(real_call_data->arena),
                                             *real_call_data->request, real_call_data->response);
    }

    return true;
  };

  call_data->response_reader =
      stub->AsyncExport(call_data->grpc_context.get(), *call_data->request, &async_data->cq);
  if (!call_data->response_reader)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << export_data_count << " " << export_data_name
                            << " failed, call AsyncExport failed");

    if (call_data->result_callback)
    {
      call_data->result_callback(
          opentelemetry::sdk::common::ExportResult::kFailure, std::move(call_data->arena),
          nullptr == call_data->request ? request : *call_data->request, call_data->response);
    }
    delete call_data;
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  call_data->response_reader->Finish(call_data->response, &call_data->grpc_status,
                                     reinterpret_cast<void *>(call_data));

  {
    ++async_data->start_request_counter;
    ++async_data->running_requests;
  }

  // Maybe spawn background thread to handle the completion queue
  MaybeSpawnBackgroundThread(async_data);

  // Can not cancle when start the request
  {
    std::unique_lock<std::mutex> lock{async_data->session_waker_lock};
    async_data->session_waker.wait_for(lock, async_data->export_timeout, [async_data]() {
      return async_data->running_requests.load(std::memory_order_acquire) <=
             async_data->max_concurrent_requests;
    });
  }

  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

}  // namespace

#ifdef ENABLE_ASYNC_EXPORT
OtlpGrpcClient::OtlpGrpcClient() : is_shutdown_{false} {}

OtlpGrpcClient::~OtlpGrpcClient()
{
  std::shared_ptr<OtlpGrpcClientAsyncData> async_data;
  async_data.swap(async_data_);

  while (async_data->running_requests.load(std::memory_order_acquire) > 0)
  {
    std::unique_lock<std::mutex> lock{async_data->session_waker_lock};
    async_data->session_waker.wait_for(lock, async_data->export_timeout, [async_data]() {
      return async_data->running_requests.load(std::memory_order_acquire) <=
             async_data->max_concurrent_requests;
    });
  }
}

#endif

std::shared_ptr<grpc::Channel> OtlpGrpcClient::MakeChannel(const OtlpGrpcClientOptions &options)
{
  std::shared_ptr<grpc::Channel> channel;

  //
  // Scheme is allowed in OTLP endpoint definition, but is not allowed for creating gRPC
  // channel. Passing URI with scheme to grpc::CreateChannel could resolve the endpoint to some
  // unexpected address.
  //

  ext::http::common::UrlParser url(options.endpoint);
  if (!url.success_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] invalid endpoint: " << options.endpoint);

    return nullptr;
  }

  std::string grpc_target = url.host_ + ":" + std::to_string(static_cast<int>(url.port_));
  grpc::ChannelArguments grpc_arguments;
  grpc_arguments.SetUserAgentPrefix(options.user_agent);

  if (options.max_threads > 0)
  {
    grpc::ResourceQuota quota;
    quota.SetMaxThreads(static_cast<int>(options.max_threads));
    grpc_arguments.SetResourceQuota(quota);
  }

  if (options.use_ssl_credentials)
  {
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = GetFileContentsOrInMemoryContents(
        options.ssl_credentials_cacert_path, options.ssl_credentials_cacert_as_string);
#if ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW
    ssl_opts.pem_private_key = GetFileContentsOrInMemoryContents(options.ssl_client_key_path,
                                                                 options.ssl_client_key_string);
    ssl_opts.pem_cert_chain  = GetFileContentsOrInMemoryContents(options.ssl_client_cert_path,
                                                                 options.ssl_client_cert_string);
#endif
    channel =
        grpc::CreateCustomChannel(grpc_target, grpc::SslCredentials(ssl_opts), grpc_arguments);
  }
  else
  {
    channel =
        grpc::CreateCustomChannel(grpc_target, grpc::InsecureChannelCredentials(), grpc_arguments);
  }

  return channel;
}

std::unique_ptr<grpc::ClientContext> OtlpGrpcClient::MakeClientContext(
    const OtlpGrpcClientOptions &options)
{
  std::unique_ptr<grpc::ClientContext> context{new grpc::ClientContext()};
  if (!context)
  {
    return context;
  }

  if (options.timeout.count() > 0)
  {
    context->set_deadline(std::chrono::system_clock::now() + options.timeout);
  }

  for (auto &header : options.metadata)
  {
    context->AddMetadata(header.first, header.second);
  }

  return context;
}

std::unique_ptr<grpc::CompletionQueue> OtlpGrpcClient::MakeCompletionQueue()
{
  return std::unique_ptr<grpc::CompletionQueue>(new grpc::CompletionQueue());
}

std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface>
OtlpGrpcClient::MakeTraceServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::trace::v1::TraceService::NewStub(MakeChannel(options));
}

std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface>
OtlpGrpcClient::MakeMetricsServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::metrics::v1::MetricsService::NewStub(MakeChannel(options));
}

std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface>
OtlpGrpcClient::MakeLogsServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::logs::v1::LogsService::NewStub(MakeChannel(options));
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::trace::v1::TraceService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> && /*arena*/,
    proto::collector::trace::v1::ExportTraceServiceRequest &&request,
    proto::collector::trace::v1::ExportTraceServiceResponse *response)
{
  return stub->Export(context.get(), request, response);
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::metrics::v1::MetricsService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> && /*arena*/,
    proto::collector::metrics::v1::ExportMetricsServiceRequest &&request,
    proto::collector::metrics::v1::ExportMetricsServiceResponse *response)
{
  return stub->Export(context.get(), request, response);
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::logs::v1::LogsService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> && /*arena*/,
    proto::collector::logs::v1::ExportLogsServiceRequest &&request,
    proto::collector::logs::v1::ExportLogsServiceResponse *response)
{
  return stub->Export(context.get(), request, response);
}

#ifdef ENABLE_ASYNC_EXPORT

/**
 * Async export
 * @param options Options used to message to create gRPC context and stub(if necessary)
 * @param arena Protobuf arena to hold lifetime of all messages
 * @param request Request for this RPC
 * @param result_callback callback to call when the exporting is done
 * @return return the status of this operation
 */
sdk::common::ExportResult OtlpGrpcClient::DelegateAsyncExport(
    const OtlpGrpcClientOptions &options,
    proto::collector::trace::v1::TraceService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> &&arena,
    proto::collector::trace::v1::ExportTraceServiceRequest &&request,
    std::function<bool(opentelemetry::sdk::common::ExportResult,
                       std::unique_ptr<google::protobuf::Arena> &&,
                       const proto::collector::trace::v1::ExportTraceServiceRequest &,
                       proto::collector::trace::v1::ExportTraceServiceResponse *)>
        &&result_callback) noexcept
{
  auto span_count = request.resource_spans_size();
  if (is_shutdown_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << span_count << " trace span(s) failed, exporter is shutdown");
    if (result_callback)
    {
      result_callback(opentelemetry::sdk::common::ExportResult::kFailure, std::move(arena), request,
                      nullptr);
    }
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  std::shared_ptr<OtlpGrpcClientAsyncData> async_data = MutableAsyncData(options);
  return InternalDelegateAsyncExport(async_data, stub, std::move(context), std::move(arena),
                                     std::move(request), std::move(result_callback), span_count,
                                     "trace span(s)");
}

sdk::common::ExportResult OtlpGrpcClient::DelegateAsyncExport(
    const OtlpGrpcClientOptions &options,
    proto::collector::metrics::v1::MetricsService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> &&arena,
    proto::collector::metrics::v1::ExportMetricsServiceRequest &&request,
    std::function<bool(opentelemetry::sdk::common::ExportResult,
                       std::unique_ptr<google::protobuf::Arena> &&,
                       const proto::collector::metrics::v1::ExportMetricsServiceRequest &,
                       proto::collector::metrics::v1::ExportMetricsServiceResponse *)>
        &&result_callback) noexcept
{
  auto metrics_count = request.resource_metrics_size();
  if (is_shutdown_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << metrics_count << " metric(s) failed, exporter is shutdown");
    if (result_callback)
    {
      result_callback(opentelemetry::sdk::common::ExportResult::kFailure, std::move(arena), request,
                      nullptr);
    }
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  std::shared_ptr<OtlpGrpcClientAsyncData> async_data = MutableAsyncData(options);
  return InternalDelegateAsyncExport(async_data, stub, std::move(context), std::move(arena),
                                     std::move(request), std::move(result_callback), metrics_count,
                                     "metric(s)");
}

sdk::common::ExportResult OtlpGrpcClient::DelegateAsyncExport(
    const OtlpGrpcClientOptions &options,
    proto::collector::logs::v1::LogsService::StubInterface *stub,
    std::unique_ptr<grpc::ClientContext> &&context,
    std::unique_ptr<google::protobuf::Arena> &&arena,
    proto::collector::logs::v1::ExportLogsServiceRequest &&request,
    std::function<bool(opentelemetry::sdk::common::ExportResult,
                       std::unique_ptr<google::protobuf::Arena> &&,
                       const proto::collector::logs::v1::ExportLogsServiceRequest &,
                       proto::collector::logs::v1::ExportLogsServiceResponse *)>
        &&result_callback) noexcept
{
  auto logs_count = request.resource_logs_size();
  if (is_shutdown_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] ERROR: Export "
                            << logs_count << " log(s) failed, exporter is shutdown");
    if (result_callback)
    {
      result_callback(opentelemetry::sdk::common::ExportResult::kFailure, std::move(arena), request,
                      nullptr);
    }
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  std::shared_ptr<OtlpGrpcClientAsyncData> async_data = MutableAsyncData(options);
  return InternalDelegateAsyncExport(async_data, stub, std::move(context), std::move(arena),
                                     std::move(request), std::move(result_callback), logs_count,
                                     "log(s)");
}

std::shared_ptr<OtlpGrpcClientAsyncData> OtlpGrpcClient::MutableAsyncData(
    const OtlpGrpcClientOptions &options)
{
  if (!async_data_)
  {
    async_data_                          = std::make_shared<OtlpGrpcClientAsyncData>();
    async_data_->export_timeout          = options.timeout;
    async_data_->max_concurrent_requests = options.max_concurrent_requests;
  }

  return async_data_;
}

bool OtlpGrpcClient::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (!async_data_)
  {
    return true;
  }

  std::size_t request_counter = async_data_->start_request_counter.load(std::memory_order_acquire);
  if (request_counter <= async_data_->finished_request_counter.load(std::memory_order_acquire))
  {
    return true;
  }

  // ASAN will report chrono: runtime error: signed integer overflow: A + B cannot be represented
  //   in type 'long int' here. So we reset timeout to meet signed long int limit here.
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());

  // Wait for all the sessions to finish
  std::unique_lock<std::mutex> lock(async_data_->session_waker_lock);

  std::chrono::steady_clock::duration timeout_steady =
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout);
  if (timeout_steady <= std::chrono::steady_clock::duration::zero())
  {
    timeout_steady = std::chrono::steady_clock::duration::max();
  }

  while (timeout_steady > std::chrono::steady_clock::duration::zero())
  {
    // When changes of running_sessions_ and notify_one/notify_all happen between predicate
    // checking and waiting, we should not wait forever.We should cleanup gc sessions here as soon
    // as possible to call FinishSession() and cleanup resources.
    std::chrono::steady_clock::time_point start_timepoint = std::chrono::steady_clock::now();
    if (std::cv_status::timeout !=
        async_data_->session_waker.wait_for(lock, async_data_->export_timeout))
    {
      break;
    }

    timeout_steady -= std::chrono::steady_clock::now() - start_timepoint;
  }

  return timeout_steady > std::chrono::steady_clock::duration::zero();
}

bool OtlpGrpcClient::Shutdown(std::chrono::microseconds timeout) noexcept
{
  if (!async_data_)
  {
    return true;
  }

  async_data_->cq.Shutdown();
  return ForceFlush(timeout);
}

#endif

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
