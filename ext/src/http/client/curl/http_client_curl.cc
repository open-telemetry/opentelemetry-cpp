// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <curl/curl.h>
#include <curl/curlver.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/curl/http_operation_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"
#include "opentelemetry/version.h"

#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
#  include <zconf.h>
#  include <zlib.h>
#  include <algorithm>
#  include <array>

#  include "opentelemetry/nostd/type_traits.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace curl
{

HttpCurlGlobalInitializer::HttpCurlGlobalInitializer()
{
  curl_global_init(CURL_GLOBAL_ALL);
}

HttpCurlGlobalInitializer::~HttpCurlGlobalInitializer()
{
  curl_global_cleanup();
}

nostd::shared_ptr<HttpCurlGlobalInitializer> HttpCurlGlobalInitializer::GetInstance()
{
  static nostd::shared_ptr<HttpCurlGlobalInitializer> shared_initializer{
      new HttpCurlGlobalInitializer()};
  return shared_initializer;
}

#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
// Original source:
// https://stackoverflow.com/questions/12398377/is-it-possible-to-have-zlib-read-from-and-write-to-the-same-memory-buffer/12412863#12412863
static int deflateInPlace(z_stream *strm, unsigned char *buf, uint32_t len, uint32_t *max_len)
{
  // must be large enough to hold zlib or gzip header (if any) and one more byte -- 11 works for the
  // worst case here, but if gzip encoding is used and a deflateSetHeader() call is inserted in this
  // code after the deflateReset(), then the 11 needs to be increased to accommodate the resulting
  // gzip header size plus one
  std::array<unsigned char, 11> temp{};

  // kick start the process with a temporary output buffer -- this allows deflate to consume a large
  // chunk of input data in order to make room for output data there
  strm->next_in  = buf;
  strm->avail_in = len;
  if (*max_len < len)
  {
    *max_len = len;
  }
  strm->next_out  = temp.data();
  strm->avail_out = (std::min)(static_cast<decltype(z_stream::avail_out)>(temp.size()), *max_len);
  auto ret        = deflate(strm, Z_FINISH);
  if (ret == Z_STREAM_ERROR)
  {
    return ret;
  }

  // if we can, copy the temporary output data to the consumed portion of the input buffer, and then
  // continue to write up to the start of the consumed input for as long as possible
  auto have = strm->next_out - temp.data();  // number of bytes in temp[]
  if (have <= static_cast<decltype(have)>(strm->avail_in ? len - strm->avail_in : *max_len))
  {
    std::memcpy(buf, temp.data(), have);
    strm->next_out = buf + have;
    have           = 0;
    while (ret == Z_OK)
    {
      strm->avail_out = static_cast<decltype(z_stream::avail_out)>(
          strm->avail_in ? strm->next_in - strm->next_out : (buf + *max_len) - strm->next_out);
      ret = deflate(strm, Z_FINISH);
    }
    if (ret != Z_BUF_ERROR || strm->avail_in == 0)
    {
      *max_len = static_cast<uint32_t>(strm->next_out - buf);
      return ret == Z_STREAM_END ? Z_OK : ret;
    }
  }

  // the output caught up with the input due to insufficiently compressible data -- copy the
  // remaining input data into an allocated buffer and complete the compression from there to the
  // now empty input buffer (this will only occur for long incompressible streams, more than ~20 MB
  // for the default deflate memLevel of 8, or when *max_len is too small and less than the length
  // of the header plus one byte)
  auto hold = static_cast<nostd::remove_const_t<decltype(z_stream::next_in)>>(
      strm->zalloc(strm->opaque, strm->avail_in, 1));  // allocated buffer to hold input data
  if (hold == Z_NULL)
  {
    return Z_MEM_ERROR;
  }
  std::memcpy(hold, strm->next_in, strm->avail_in);
  strm->next_in = hold;
  if (have)
  {
    std::memcpy(buf, temp.data(), have);
    strm->next_out = buf + have;
  }
  strm->avail_out = static_cast<decltype(z_stream::avail_out)>((buf + *max_len) - strm->next_out);
  ret             = deflate(strm, Z_FINISH);
  strm->zfree(strm->opaque, hold);
  *max_len = static_cast<uint32_t>(strm->next_out - buf);
  return ret == Z_OK ? Z_BUF_ERROR : (ret == Z_STREAM_END ? Z_OK : ret);
}
#endif  // ENABLE_OTLP_COMPRESSION_PREVIEW

void Session::SendRequest(
    std::shared_ptr<opentelemetry::ext::http::client::EventHandler> callback) noexcept
{
  is_session_active_.store(true, std::memory_order_release);
  const auto &url       = host_ + http_request_->uri_;
  auto callback_ptr     = callback.get();
  bool reuse_connection = false;

  // Set CURLOPT_FRESH_CONNECT and CURLOPT_FORBID_REUSE to 1L every max_sessions_per_connection_
  // requests. So libcurl will create a new connection instead of queue the request to the existing
  // connection.
  if (http_client_.GetMaxSessionsPerConnection() > 0)
  {
    reuse_connection = session_id_ % http_client_.GetMaxSessionsPerConnection() != 0;
  }

  if (http_request_->compression_ == opentelemetry::ext::http::client::Compression::kGzip)
  {
#ifdef ENABLE_OTLP_COMPRESSION_PREVIEW
    z_stream zs{};
    zs.zalloc = Z_NULL;
    zs.zfree  = Z_NULL;
    zs.opaque = Z_NULL;

    // ZLIB: Have to maually specify 16 bits for the Gzip headers
    static constexpr int kWindowBits = MAX_WBITS + 16;
    static constexpr int kMemLevel   = MAX_MEM_LEVEL;

    auto stream = deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, kWindowBits, kMemLevel,
                               Z_DEFAULT_STRATEGY);

    if (stream == Z_OK)
    {
      auto size     = static_cast<uInt>(http_request_->body_.size());
      auto max_size = size;
      stream        = deflateInPlace(&zs, http_request_->body_.data(), size, &max_size);

      if (stream == Z_OK)
      {
        http_request_->AddHeader("Content-Encoding", "gzip");
        http_request_->body_.resize(max_size);
      }
    }

    if (stream != Z_OK)
    {
      if (callback)
      {
        callback->OnEvent(opentelemetry::ext::http::client::SessionState::CreateFailed,
                          zs.msg ? zs.msg : "");
      }
      is_session_active_.store(false, std::memory_order_release);
    }

    deflateEnd(&zs);
#else
    OTEL_INTERNAL_LOG_ERROR(
        "[HTTP Client Curl] Set WITH_OTLP_HTTP_COMPRESSION=ON to use gzip compression with the "
        "OTLP HTTP Exporter");
#endif  // ENABLE_OTLP_COMPRESSION_PREVIEW
  }

  curl_operation_.reset(
      new HttpOperation(http_request_->method_, url, http_request_->ssl_options_, callback_ptr,
                        http_request_->headers_, http_request_->body_, http_request_->compression_,
                        false, http_request_->timeout_ms_, reuse_connection,
                        http_request_->is_log_enabled_, http_request_->retry_policy_));
  bool success =
      CURLE_OK == curl_operation_->SendAsync(this, [this, callback](HttpOperation &operation) {
        // Both can hold at once: cancelling only raises a flag, and the transfer may already
        // have been answered. A request that was answered is reported as answered.
        if (operation.GetSessionState() == opentelemetry::ext::http::client::SessionState::Response)
        {
          // we have a http response
          auto response          = std::unique_ptr<Response>(new Response());
          response->headers_     = operation.GetResponseHeaders();
          response->body_        = operation.GetResponseBody();
          response->status_code_ = operation.GetResponseCode();
          callback->OnResponse(*response);
        }
        else if (operation.WasAborted())
        {
          // Manually cancelled
          callback->OnEvent(opentelemetry::ext::http::client::SessionState::Cancelled, "");
        }
        is_session_active_.store(false, std::memory_order_release);
      });

  if (success)
  {
    // We will try to create a background to poll events. But when the background is running, we
    // will reuse it instead of creating a new one.
    http_client_.MaybeSpawnBackgroundThread();
  }
  else
  {
    if (callback)
    {
      callback->OnEvent(opentelemetry::ext::http::client::SessionState::CreateFailed,
                        curl_easy_strerror(curl_operation_->GetLastResultCode()));
    }
    is_session_active_.store(false, std::memory_order_release);
  }
}

bool Session::CancelSession() noexcept
{
  if (curl_operation_)
  {
    curl_operation_->Abort();
  }
  http_client_.CleanupSession(session_id_);
  return true;
}

bool Session::FinishSession() noexcept
{
  if (curl_operation_)
  {
    curl_operation_->Finish();
  }
  http_client_.CleanupSession(session_id_);
  return true;
}

void Session::FinishOperation()
{
  if (curl_operation_)
  {
    curl_operation_->Cleanup();
  }
}

// Reported once, where it happens. The IO loop does its own reporting, because sharing this one
// would repeat the same line on every pass for as long as the handle stays missing.
static CURLM *initMultiHandle()
{
  CURLM *handle = curl_multi_init();
  if (nullptr == handle)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[HTTP Client Curl] curl_multi_init failed, requests cannot be processed until it "
        "succeeds");
  }
  return handle;
}

HttpClient::HttpClient()
    : multi_handle_(nullptr),
      next_session_id_{0},
      max_sessions_per_connection_{8},
      background_thread_instrumentation_(nullptr),
      scheduled_delay_milliseconds_{std::chrono::milliseconds(256)},
      background_thread_wait_for_{std::chrono::minutes{1}},
      curl_global_initializer_(HttpCurlGlobalInitializer::GetInstance())
{
  // Not in the initialiser list: curl_global_initializer_ is declared later and has to run first.
  multi_handle_ = initMultiHandle();
}

HttpClient::HttpClient(
    const std::shared_ptr<sdk::common::ThreadInstrumentation> &thread_instrumentation)
    : multi_handle_(nullptr),
      next_session_id_{0},
      max_sessions_per_connection_{8},
      background_thread_instrumentation_(thread_instrumentation),
      scheduled_delay_milliseconds_{std::chrono::milliseconds(256)},
      background_thread_wait_for_{std::chrono::minutes{1}},
      curl_global_initializer_(HttpCurlGlobalInitializer::GetInstance())
{
  // Not in the initialiser list: curl_global_initializer_ is declared later and has to run first.
  multi_handle_ = initMultiHandle();
}

HttpClient::~HttpClient()
{
  is_shutdown_.store(true, std::memory_order_release);
  while (true)
  {
    std::unique_ptr<std::thread> background_thread;
    {
      std::lock_guard<std::mutex> lock_guard{background_thread_m_};
      background_thread.swap(background_thread_);
    }

    // Force to abort all sessions
    InternalCancelAllSessions();

    if (!background_thread)
    {
      break;
    }
    if (background_thread->joinable())
    {
      wakeupBackgroundThread();  // if delay quit, wake up first
      background_thread->join();
    }
  }
  {
    std::lock_guard<std::mutex> lock_guard{multi_handle_m_};
    ReleaseMultiHandle();
  }
}

std::shared_ptr<opentelemetry::ext::http::client::Session> HttpClient::CreateSession(
    nostd::string_view url) noexcept
{
  const auto parsedUrl = common::UrlParser(std::string(url));
  if (!parsedUrl.success_)
  {
    return std::make_shared<Session>(*this);
  }
  auto session =
      std::make_shared<Session>(*this, parsedUrl.scheme_, parsedUrl.host_, parsedUrl.port_);
  auto session_id = ++next_session_id_;
  session->SetId(session_id);

  std::lock_guard<std::mutex> lock_guard{sessions_m_};
  sessions_.insert({session_id, session});

  // FIXME: Session may leak if it does not call SendRequest
  return session;
}

bool HttpClient::CancelAllSessions() noexcept
{
  return InternalCancelAllSessions();
}

bool HttpClient::FinishAllSessions() noexcept
{
  // FinishSession may change sessions_, we can not change a container while iterating it.
  while (true)
  {
    std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions;
    {
      // We can only cleanup session and curl handles in the IO thread.
      std::lock_guard<std::mutex> lock_guard{sessions_m_};
      sessions = sessions_;
    }

    if (sessions.empty())
    {
      break;
    }

    for (auto &session : sessions)
    {
      session.second->FinishSession();
    }
  }
  return true;
}

void HttpClient::SetMaxSessionsPerConnection(std::size_t max_requests_per_connection) noexcept
{
  max_sessions_per_connection_ = max_requests_per_connection;
}

void HttpClient::CleanupSession(uint64_t session_id)
{
  std::shared_ptr<Session> session;
  {
    std::lock_guard<std::mutex> lock_guard{sessions_m_};
    auto it = sessions_.find(session_id);
    if (it != sessions_.end())
    {
      session = it->second;
      sessions_.erase(it);
    }
  }

  bool need_wakeup_background_thread = false;
  {
    std::lock_guard<std::recursive_mutex> lock_guard{session_ids_m_};
    pending_to_add_session_ids_.erase(session_id);

    if (session)
    {
      if (pending_to_remove_session_handles_.end() !=
          pending_to_remove_session_handles_.find(session_id))
      {
        pending_to_remove_sessions_.emplace_back(std::move(session));
      }
      else if (session->IsSessionActive() && session->GetOperation())
      {
        // If this session is already running, give it to the background thread for cleanup.
        pending_to_abort_sessions_[session_id] = std::move(session);
        need_wakeup_background_thread          = true;
      }
    }
  }

  if (need_wakeup_background_thread)
  {
    wakeupBackgroundThread();
  }
}

bool HttpClient::InternalCancelAllSessions() noexcept
{
  // CancelSession may change sessions_, we can not change a container while iterating it.
  while (true)
  {
    std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions;
    {
      // We can only cleanup session and curl handles in the IO thread.
      std::lock_guard<std::mutex> lock_guard{sessions_m_};
      sessions = sessions_;
    }

    if (sessions.empty())
    {
      break;
    }

    for (auto &session : sessions)
    {
      session.second->CancelSession();
    }
  }
  return true;
}

bool HttpClient::MaybeSpawnBackgroundThread()
{
  std::lock_guard<std::mutex> lock_guard{background_thread_m_};
  if (background_thread_)
  {
    return false;
  }

  background_thread_.reset(new std::thread(
      [](HttpClient *self) {
#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
        if (self->background_thread_instrumentation_ != nullptr)
        {
          self->background_thread_instrumentation_->OnStart();
        }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */

        auto still_running                 = 1;
        auto last_free_job_timepoint       = std::chrono::system_clock::now();
        auto need_wait_more                = false;
        bool missing_multi_handle_reported = false;
        while (true)
        {
          CURLMsg *msg = nullptr;
          int queued   = 0;
          // curl_multi_init says the other multi functions cannot be used once it has returned
          // null, so a missing handle is answered here rather than passed to libcurl.
          CURLMcode mc = CURLM_BAD_HANDLE;
          if (nullptr != self->multi_handle_)
          {
            mc = curl_multi_perform(self->multi_handle_, &still_running);
          }
          // According to https://curl.se/libcurl/c/curl_multi_perform.html, when mc is not OK, we
          // can not curl_multi_perform it again
          if (mc != CURLM_OK)
          {
            // curl_multi_perform leaves still_running alone when it rejects the handle, and it
            // starts at one, so without this the loop keeps reporting work it does not have,
            // never reaches the shutdown check below, and the thread cannot be joined.
            still_running           = 0;
            const uint64_t woken_at = self->wakeup_generation_.load(std::memory_order_acquire);
            if (self->resetMultiHandle())
            {
              missing_multi_handle_reported = false;
            }
            else if (!self->is_shutdown_.load(std::memory_order_acquire))
            {
              // Nothing can run without a handle. Retrying at once pegs a core and repeats one
              // error for the whole idle window, so report the run of failures once and wait as
              // long as a poll would have. Shutdown skips the wait so teardown stays prompt.
              if (!missing_multi_handle_reported)
              {
                OTEL_INTERNAL_LOG_ERROR(
                    "[HTTP Client Curl] no multi handle, requests cannot be processed until "
                    "curl_multi_init succeeds");
                missing_multi_handle_reported = true;
              }
#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
              if (self->background_thread_instrumentation_ != nullptr)
              {
                self->background_thread_instrumentation_->BeforeWait();
              }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */

              // In slices, because curl_multi_wakeup cannot reach this thread: it goes through
              // the multi handle, and there is not one. What ends the wait early instead is the
              // counter every producer raises, or shutdown. A whole delay spent either way would
              // be a whole delay added to answering the next request, and to destroying the
              // client.
              constexpr std::chrono::milliseconds kMissingHandleWaitSlice{16};
              for (std::chrono::milliseconds waited = std::chrono::milliseconds::zero();
                   waited < self->scheduled_delay_milliseconds_ &&
                   !self->is_shutdown_.load(std::memory_order_acquire) &&
                   woken_at == self->wakeup_generation_.load(std::memory_order_acquire);
                   waited += kMissingHandleWaitSlice)
              {
                std::this_thread::sleep_for(kMissingHandleWaitSlice);
              }

#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
              if (self->background_thread_instrumentation_ != nullptr)
              {
                self->background_thread_instrumentation_->AfterWait();
              }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */
            }
          }
          else if (still_running || need_wait_more)
          {
#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
            if (self->background_thread_instrumentation_ != nullptr)
            {
              self->background_thread_instrumentation_->BeforeWait();
            }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */

        // curl_multi_poll is added from libcurl 7.66.0, before 7.68.0, we can only wait until
        // timeout to do the remaining jobs
#if LIBCURL_VERSION_NUM >= 0x074200
            /* wait for activity, timeout or "nothing" */
            mc = curl_multi_poll(self->multi_handle_, nullptr, 0,
                                 static_cast<int>(self->scheduled_delay_milliseconds_.count()),
                                 nullptr);
#else
            mc = curl_multi_wait(self->multi_handle_, nullptr, 0,
                                 static_cast<int>(self->scheduled_delay_milliseconds_.count()),
                                 nullptr);
#endif

#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
            if (self->background_thread_instrumentation_ != nullptr)
            {
              self->background_thread_instrumentation_->AfterWait();
            }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */
          }

          do
          {
            msg = (nullptr == self->multi_handle_)
                      ? nullptr
                      : curl_multi_info_read(self->multi_handle_, &queued);
            if (msg == nullptr)
            {
              break;
            }

            if (msg->msg == CURLMSG_DONE)
            {
              CURL *easy_handle = msg->easy_handle;
              CURLcode result   = msg->data.result;
              Session *session  = nullptr;
              curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &session);
              const auto operation = (nullptr != session) ? session->GetOperation().get() : nullptr;

              // If it's already moved into pending_to_remove_session_handles_, we just ignore this
              // message.
              if (operation)
              {
                // Session can not be destroyed when calling PerformCurlMessage
                auto hold_session = session->shared_from_this();
                operation->PerformCurlMessage(result);

                if (operation->IsRetryable())
                {
                  self->pending_to_retry_sessions_.push_back(hold_session);
                }
              }
            }
          } while (true);

          // Abort all pending easy handles. Calls no multi function, so it runs without a handle.
          if (self->doAbortSessions())
          {
            still_running = 1;
          }

          // Remove all pending easy handles. Detaching is the only thing here that wants a
          // multi handle, and without one there is nothing to detach from, so this releases
          // rather than waits: holding the resources back would hold them for the whole
          // outage.
          if (self->doRemoveSessions())
          {
            still_running = 1;
          }

          // Add all pending easy handles. Answers for itself when there is no handle, as does
          // the retry below: neither can hand libcurl a transfer without one.
          if (self->doAddSessions())
          {
            still_running = 1;
          }

          // Check if pending easy handles can be retried
          if (self->doRetrySessions(false))
          {
            still_running = 1;
          }

          std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
          if (still_running > 0)
          {
            last_free_job_timepoint = now;
            need_wait_more          = false;
            continue;
          }

          std::chrono::milliseconds wait_for = std::chrono::milliseconds::zero();

#if LIBCURL_VERSION_NUM >= 0x074400
          // only available with curl_multi_poll+curl_multi_wakeup, because curl_multi_wait would
          // cause CPU busy, curl_multi_wait+sleep could not wakeup quickly
          wait_for = self->background_thread_wait_for_;
#endif
          if (self->is_shutdown_.load(std::memory_order_acquire))
          {
            wait_for = std::chrono::milliseconds::zero();
          }

          if (now - last_free_job_timepoint < wait_for)
          {
            need_wait_more = true;
            continue;
          }

          if (still_running == 0)
          {
            std::lock_guard<std::mutex> lock_guard{self->background_thread_m_};
            // Double check, make sure no more pending sessions after locking background thread
            // management

            // Abort all pending easy handles
            if (self->doAbortSessions())
            {
              still_running = 1;
            }

            // Remove all pending easy handles
            if (self->doRemoveSessions())
            {
              still_running = 1;
            }

            // Add all pending easy handles
            if (self->doAddSessions())
            {
              still_running = 1;
            }

            // Check if pending easy handles can be retried
            if (self->doRetrySessions(true))
            {
              still_running = 1;
            }

            // Skipping those three reports nothing, which is not the same as having nothing to
            // do. A request the client has accepted has to be either handed to libcurl or
            // finished, and this thread is the only one that does either, so it stays while it
            // owes one. Shutdown is exempt: there the queues that need a handle cannot drain
            // without one, and staying for them is staying under the join that is waiting here.
            if (nullptr == self->multi_handle_ &&
                !self->is_shutdown_.load(std::memory_order_acquire) && self->hasActionableWork())
            {
              still_running = 1;
            }

            // If there is no pending jobs, we can stop the background thread.
            if (still_running == 0)
            {
#ifdef ENABLE_THREAD_INSTRUMENTATION_PREVIEW
              if (self->background_thread_instrumentation_ != nullptr)
              {
                self->background_thread_instrumentation_->OnEnd();
              }
#endif /* ENABLE_THREAD_INSTRUMENTATION_PREVIEW */

              if (self->background_thread_)
              {
                self->background_thread_->detach();
                self->background_thread_.reset();
              }
              break;
            }
          }
        }
      },
      this));
  return true;
}

void HttpClient::ScheduleAddSession(uint64_t session_id)
{
  {
    std::lock_guard<std::recursive_mutex> lock_guard{session_ids_m_};
    pending_to_add_session_ids_.insert(session_id);
    pending_to_remove_session_handles_.erase(session_id);
    pending_to_abort_sessions_.erase(session_id);
  }

  wakeupBackgroundThread();
}

void HttpClient::ScheduleAbortSession(uint64_t session_id)
{
  {
    std::lock_guard<std::mutex> sessions_lock{sessions_m_};
    auto session = sessions_.find(session_id);
    if (session == sessions_.end())
    {
      std::lock_guard<std::recursive_mutex> session_ids_lock{session_ids_m_};
      pending_to_add_session_ids_.erase(session_id);
    }
    else
    {
      std::lock_guard<std::recursive_mutex> session_ids_lock{session_ids_m_};
      pending_to_abort_sessions_[session_id] = std::move(session->second);
      pending_to_add_session_ids_.erase(session_id);

      sessions_.erase(session);
    }
  }

  wakeupBackgroundThread();
}

void HttpClient::ScheduleRemoveSession(uint64_t session_id, HttpCurlEasyResource &&resource)
{
  {
    std::lock_guard<std::recursive_mutex> lock_guard{session_ids_m_};
    pending_to_add_session_ids_.erase(session_id);
    pending_to_remove_session_handles_[session_id] = std::move(resource);
  }

  wakeupBackgroundThread();
}

void HttpClient::SetBackgroundWaitFor(std::chrono::milliseconds ms)
{
  background_thread_wait_for_ = ms;
}

void HttpClient::WaitBackgroundThreadExit()
{
  is_shutdown_.store(true, std::memory_order_release);
  std::unique_ptr<std::thread> background_thread;
  {
    std::lock_guard<std::mutex> lock_guard{background_thread_m_};
    background_thread.swap(background_thread_);
  }

  if (background_thread && background_thread->joinable())
  {
    wakeupBackgroundThread();
    background_thread->join();
  }
  is_shutdown_.store(false, std::memory_order_release);
}

void HttpClient::wakeupBackgroundThread()
{
  // First, and whatever libcurl is: the call below needs a multi handle and there is not always
  // one, so this is what the background thread watches when it is waiting without a handle.
  wakeup_generation_.fetch_add(1, std::memory_order_release);

// Before libcurl 7.68.0, we can only wait for timeout and do the rest jobs
// See https://curl.se/libcurl/c/curl_multi_wakeup.html
#if LIBCURL_VERSION_NUM >= 0x074400
  std::lock_guard<std::mutex> lock_guard{multi_handle_m_};
  if (nullptr != multi_handle_)
  {
    curl_multi_wakeup(multi_handle_);
  }
#endif
}

bool HttpClient::doAddSessions()
{
  if (nullptr == multi_handle_)
  {
    // Before the swap below, which would drop the ids it took.
    return false;
  }

  std::unordered_set<uint64_t> pending_to_add_session_ids;
  {
    std::lock_guard<std::recursive_mutex> session_id_lock_guard{session_ids_m_};
    pending_to_add_session_ids_.swap(pending_to_add_session_ids);
  }

  bool has_data = false;

  std::lock_guard<std::mutex> lock_guard{sessions_m_};
  for (auto &session_id : pending_to_add_session_ids)
  {
    auto session = sessions_.find(session_id);
    if (session == sessions_.end())
    {
      continue;
    }

    if (!session->second->GetOperation())
    {
      continue;
    }

    CURL *easy_handle = session->second->GetOperation()->GetCurlEasyHandle();
    if (nullptr == easy_handle)
    {
      continue;
    }

    curl_multi_add_handle(multi_handle_, easy_handle);
    has_data = true;
  }

  return has_data;
}

bool HttpClient::doAbortSessions()
{
  std::unordered_map<uint64_t, std::shared_ptr<Session>> pending_to_abort_sessions;
  {
    std::lock_guard<std::recursive_mutex> session_id_lock_guard{session_ids_m_};
    pending_to_abort_sessions_.swap(pending_to_abort_sessions);
  }

  bool has_data = false;
  for (const auto &session : pending_to_abort_sessions)
  {
    if (!session.second)
    {
      continue;
    }

    if (session.second->GetOperation())
    {
      session.second->FinishOperation();
      has_data = true;
    }
  }
  return has_data;
}

bool HttpClient::doRemoveSessions()
{
  bool has_data{false};
  bool should_continue{false};
  do
  {
    std::unordered_map<uint64_t, HttpCurlEasyResource> pending_to_remove_session_handles;
    std::list<std::shared_ptr<Session>> pending_to_remove_sessions;
    {
      std::lock_guard<std::recursive_mutex> session_id_lock_guard{session_ids_m_};
      pending_to_remove_session_handles_.swap(pending_to_remove_session_handles);
      pending_to_remove_sessions_.swap(pending_to_remove_sessions);
    }
    {
      // If user callback do not call CancelSession or FinishSession, We still need to remove it
      // from sessions_
      std::lock_guard<std::mutex> session_lock_guard{sessions_m_};
      for (auto &removing_handle : pending_to_remove_session_handles)
      {
        auto session = sessions_.find(removing_handle.first);
        if (session != sessions_.end())
        {
          pending_to_remove_sessions.emplace_back(std::move(session->second));
          sessions_.erase(session);
        }
      }
    }

    for (auto &removing_handle : pending_to_remove_session_handles)
    {
      if (nullptr != removing_handle.second.headers_chunk)
      {
        curl_slist_free_all(removing_handle.second.headers_chunk);
      }

      // Detaching needs something to detach from. Without a multi handle there is nothing
      // this could name: the one it would have named was destroyed by curl_multi_cleanup,
      // which detaches what it still holds, and nothing has been attached since. So the
      // resource is released rather than kept, which is what resetMultiHandle asks for when
      // curl_multi_init has just failed on it.
      if (nullptr != multi_handle_)
      {
        curl_multi_remove_handle(multi_handle_, removing_handle.second.easy_handle);
      }
      curl_easy_cleanup(removing_handle.second.easy_handle);
    }

    for (auto &removing_session : pending_to_remove_sessions)
    {
      // This operation may add more pending_to_remove_session_handles
      removing_session->FinishOperation();
    }

    should_continue =
        !pending_to_remove_session_handles.empty() || !pending_to_remove_sessions.empty();
    if (should_continue)
    {
      has_data = true;
    }
  } while (should_continue);

  return has_data;
}

#ifdef ENABLE_OTLP_RETRY_PREVIEW
bool HttpClient::doRetrySessions(bool report_all)
{
  if (nullptr == multi_handle_)
  {
    return false;
  }

  const auto now = std::chrono::system_clock::now();
  auto has_data  = false;

  // Assumptions:
  // - This is a FIFO list so older sessions, pushed at the back, always end up at the front
  // - Locking not required because only the background thread would be pushing to this container
  // - Retry policy is not changed once HTTP client is initialized, so same settings for everyone
  for (auto retry_it = pending_to_retry_sessions_.cbegin();
       retry_it != pending_to_retry_sessions_.cend();)
  {
    const auto session   = *retry_it;
    const auto operation = session ? session->GetOperation().get() : nullptr;

    if (!operation)
    {
      retry_it = pending_to_retry_sessions_.erase(retry_it);
    }
    else if (operation->NextRetryTime() < now)
    {
      auto easy_handle = operation->GetCurlEasyHandle();
      curl_multi_remove_handle(multi_handle_, easy_handle);
      curl_multi_add_handle(multi_handle_, easy_handle);
      retry_it = pending_to_retry_sessions_.erase(retry_it);
      has_data = true;
    }
    else
    {
      break;
    }
  }

  report_all = report_all && !pending_to_retry_sessions_.empty();
  return has_data || report_all;
}
#else
bool HttpClient::doRetrySessions(bool /* report_all */)
{
  return false;
}
#endif  // ENABLE_OTLP_RETRY_PREVIEW

void HttpClient::ReleaseMultiHandle()
{
  if (nullptr == multi_handle_)
  {
    // curl_multi_init says the other multi functions cannot be used once it has returned null,
    // and curl_multi_cleanup is one of them. Reaching here with none is ordinary: the
    // constructor may have started without one, and a reset that could not build a replacement
    // leaves none behind.
    return;
  }

  const CURLMcode cleanup_result = curl_multi_cleanup(multi_handle_);
  multi_handle_                  = nullptr;
  if (CURLM_OK != cleanup_result)
  {
    OTEL_INTERNAL_LOG_ERROR("[HTTP Client Curl] curl_multi_cleanup failed with message: "
                            << curl_multi_strerror(cleanup_result));
  }
}

bool HttpClient::hasActionableWork()
{
  std::lock_guard<std::mutex> session_lock_guard{sessions_m_};
  std::lock_guard<std::recursive_mutex> session_id_lock_guard{session_ids_m_};

  // An id whose session has gone is what doAddSessions would drop on its next pass, so it is
  // dropped here too rather than counted. The difference matters: the first version of this
  // check took every queue at its size, and an entry nothing could be done about held the
  // thread open against the join in the destructor.
  for (auto id = pending_to_add_session_ids_.begin(); id != pending_to_add_session_ids_.end();)
  {
    const auto session = sessions_.find(*id);
    if (session == sessions_.end() || !session->second || !session->second->GetOperation())
    {
      id = pending_to_add_session_ids_.erase(id);
    }
    else
    {
      ++id;
    }
  }

  // Same rule doRetrySessions applies to the same container.
  for (auto retry = pending_to_retry_sessions_.begin(); retry != pending_to_retry_sessions_.end();)
  {
    if (!*retry || !(*retry)->GetOperation())
    {
      retry = pending_to_retry_sessions_.erase(retry);
    }
    else
    {
      ++retry;
    }
  }

  return !pending_to_add_session_ids_.empty() || !pending_to_abort_sessions_.empty() ||
         !pending_to_remove_session_handles_.empty() || !pending_to_remove_sessions_.empty() ||
         !pending_to_retry_sessions_.empty();
}

bool HttpClient::resetMultiHandle()
{
  std::list<std::shared_ptr<Session>> sessions;
  {
    // Only the snapshot needs these. CancelSession and doRemoveSessions below both take
    // sessions_m_ again, and it is not recursive, so holding it across them stops the IO
    // thread here for good. Cleanup also runs the caller's handler, which this lock was
    // never meant to cover.
    std::lock_guard<std::mutex> session_lock_guard{sessions_m_};
    std::lock_guard<std::recursive_mutex> session_id_lock_guard{session_ids_m_};
    for (auto &session : sessions_)
    {
      if (pending_to_add_session_ids_.end() == pending_to_add_session_ids_.find(session.first))
      {
        sessions.push_back(session.second);
      }
    }
  }

  for (auto &session : sessions)
  {
    session->CancelSession();
    session->FinishOperation();
  }

  doRemoveSessions();

  // We will modify the multi_handle_, so we need to lock it
  std::lock_guard<std::mutex> lock_guard{multi_handle_m_};
  ReleaseMultiHandle();

  // Create a another multi handle to continue pending sessions. Silent on failure: the caller
  // decides how often a run of failures is worth reporting.
  multi_handle_ = curl_multi_init();
  return nullptr != multi_handle_;
}

}  // namespace curl
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
