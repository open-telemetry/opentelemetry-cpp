// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstring>

#include "opentelemetry/ext/http/client/curl/http_operation_curl.h"

#include "opentelemetry/ext/http/client/curl/http_client_curl.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace curl
{

size_t HttpOperation::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(userp);
  if (nullptr == self)
  {
    return 0;
  }

  self->raw_response_.insert(self->raw_response_.end(), static_cast<char *>(contents),
                             static_cast<char *>(contents) + (size * nmemb));

  if (self->WasAborted())
  {
    return 0;
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
  }

  return size * nmemb;
}

size_t HttpOperation::WriteVectorHeaderCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(userp);
  if (nullptr == self)
  {
    return 0;
  }

  const unsigned char *begin = (unsigned char *)(ptr);
  const unsigned char *end   = begin + size * nmemb;
  self->response_headers_.insert(self->response_headers_.end(), begin, end);

  if (self->WasAborted())
  {
    return 0;
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
  }

  return size * nmemb;
}

size_t HttpOperation::WriteVectorBodyCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(userp);
  if (nullptr == self)
  {
    return 0;
  }

  const unsigned char *begin = (unsigned char *)(ptr);
  const unsigned char *end   = begin + size * nmemb;
  self->response_body_.insert(self->response_body_.end(), begin, end);

  if (self->WasAborted())
  {
    return 0;
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
  }

  return size * nmemb;
}

size_t HttpOperation::ReadMemoryCallback(char *buffer, size_t size, size_t nitems, void *userp)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(userp);
  if (nullptr == self)
  {
    return 0;
  }

  if (self->WasAborted())
  {
    return CURL_READFUNC_ABORT;
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
  }

  // EOF
  if (self->request_nwrite_ >= self->request_body_.size())
  {
    return 0;
  }

  size_t nwrite = size * nitems;
  if (nwrite > self->request_body_.size() - self->request_nwrite_)
  {
    nwrite = self->request_body_.size() - self->request_nwrite_;
  }

  memcpy(buffer, &self->request_body_[self->request_nwrite_], nwrite);
  self->request_nwrite_ += nwrite;
  return nwrite;
}

#if LIBCURL_VERSION_NUM >= 0x075000
int HttpOperation::PreRequestCallback(void *clientp, char *, char *, int, int)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(clientp);
  if (nullptr == self)
  {
    return CURL_PREREQFUNC_ABORT;
  }

  if (self->GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    self->DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (self->WasAborted())
  {
    return CURL_PREREQFUNC_ABORT;
  }

  return CURL_PREREQFUNC_OK;
}
#endif

#if LIBCURL_VERSION_NUM >= 0x072000
int HttpOperation::OnProgressCallback(void *clientp,
                                      curl_off_t /* dltotal */,
                                      curl_off_t /* dlnow */,
                                      curl_off_t /* ultotal */,
                                      curl_off_t /* ulnow */)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(clientp);
  if (nullptr == self)
  {
    return -1;
  }

  if (self->WasAborted())
  {
    return -1;
  }

  // CURL_PROGRESSFUNC_CONTINUE is added in 7.68.0
#  if defined(CURL_PROGRESSFUNC_CONTINUE)
  return CURL_PROGRESSFUNC_CONTINUE;
#  else
  return 0;
#  endif
}
#else
int HttpOperation::OnProgressCallback(void *clientp,
                                      double /* dltotal */,
                                      double /* dlnow */,
                                      double /* ultotal */,
                                      double /* ulnow */)
{
  HttpOperation *self = reinterpret_cast<HttpOperation *>(clientp);
  if (nullptr == self)
  {
    return -1;
  }

  if (self->WasAborted())
  {
    return -1;
  }

  return 0;
}
#endif

void HttpOperation::DispatchEvent(opentelemetry::ext::http::client::SessionState type,
                                  std::string reason)
{
  if (event_handle_ != nullptr)
  {
    event_handle_->OnEvent(type, reason);
  }

  session_state_ = type;
}

HttpOperation::HttpOperation(opentelemetry::ext::http::client::Method method,
                             std::string url,
                             opentelemetry::ext::http::client::EventHandler *event_handle,
                             // Default empty headers and empty request body
                             const opentelemetry::ext::http::client::Headers &request_headers,
                             const opentelemetry::ext::http::client::Body &request_body,
                             // Default connectivity and response size options
                             bool is_raw_response,
                             std::chrono::milliseconds http_conn_timeout,
                             bool reuse_connection)
    : is_aborted_(false),
      is_finished_(false),
      is_cleaned_(false),
      // Optional connection params
      is_raw_response_(is_raw_response),
      reuse_connection_(reuse_connection),
      http_conn_timeout_(http_conn_timeout),
      // Result
      last_curl_result_(CURLE_OK),
      event_handle_(event_handle),
      method_(method),
      url_(url),
      // Local vars
      request_headers_(request_headers),
      request_body_(request_body),
      request_nwrite_(0),
      session_state_(opentelemetry::ext::http::client::SessionState::Created),
      response_code_(0)
{
  /* get a curl handle */
  curl_resource_.easy_handle = curl_easy_init();
  if (!curl_resource_.easy_handle)
  {
    last_curl_result_ = CURLE_FAILED_INIT;
    DispatchEvent(opentelemetry::ext::http::client::SessionState::CreateFailed,
                  curl_easy_strerror(last_curl_result_));
    return;
  }

  // Specify our custom headers
  if (!this->request_headers_.empty())
  {
    for (auto &kv : this->request_headers_)
    {
      std::string header = std::string(kv.first);
      header += ": ";
      header += std::string(kv.second);
      curl_resource_.headers_chunk =
          curl_slist_append(curl_resource_.headers_chunk, header.c_str());
    }
  }

  DispatchEvent(opentelemetry::ext::http::client::SessionState::Created);
}

HttpOperation::~HttpOperation()
{
  // Given the request has not been aborted we should wait for completion here
  // This guarantees the lifetime of this request.
  switch (GetSessionState())
  {
    case opentelemetry::ext::http::client::SessionState::Connecting:
    case opentelemetry::ext::http::client::SessionState::Connected:
    case opentelemetry::ext::http::client::SessionState::Sending: {
      if (async_data_ && async_data_->result_future.valid())
      {
        if (async_data_->callback_thread != std::this_thread::get_id())
        {
          async_data_->result_future.wait();
          last_curl_result_ = async_data_->result_future.get();
        }
      }
      break;
    }
    default:
      break;
  }

  Cleanup();
}

void HttpOperation::Finish()
{
  if (is_finished_.exchange(true, std::memory_order_acq_rel))
  {
    return;
  }

  if (async_data_ && async_data_->result_future.valid())
  {
    // We should not wait in callback from Cleanup()
    if (async_data_->callback_thread != std::this_thread::get_id())
    {
      async_data_->result_future.wait();
      last_curl_result_ = async_data_->result_future.get();
    }
  }
}

void HttpOperation::Cleanup()
{
  if (is_cleaned_.exchange(true, std::memory_order_acq_rel))
  {
    return;
  }

  switch (GetSessionState())
  {
    case opentelemetry::ext::http::client::SessionState::Created:
    case opentelemetry::ext::http::client::SessionState::Connecting:
    case opentelemetry::ext::http::client::SessionState::Connected:
    case opentelemetry::ext::http::client::SessionState::Sending: {
      DispatchEvent(opentelemetry::ext::http::client::SessionState::Cancelled,
                    curl_easy_strerror(last_curl_result_));
      break;
    }
    default:
      break;
  }

  std::function<void(HttpOperation &)> callback;

  // Only cleanup async once even in recursive calls
  if (async_data_)
  {
    // Just reset and move easy_handle to owner if in async mode
    if (async_data_->session != nullptr)
    {
      auto session         = async_data_->session;
      async_data_->session = nullptr;

      if (curl_resource_.easy_handle != nullptr)
      {
        curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PRIVATE, NULL);
        curl_easy_reset(curl_resource_.easy_handle);
      }
      session->GetHttpClient().ScheduleRemoveSession(session->GetSessionId(),
                                                     std::move(curl_resource_));
    }

    callback.swap(async_data_->callback);
    if (callback)
    {
      async_data_->callback_thread = std::this_thread::get_id();
      callback(*this);
      async_data_->callback_thread = std::thread::id();
    }

    // Set value to promise to continue Finish()
    if (true == async_data_->is_promise_running.exchange(false, std::memory_order_acq_rel))
    {
      async_data_->result_promise.set_value(last_curl_result_);
    }

    return;
  }

  // Sync mode
  if (curl_resource_.easy_handle != nullptr)
  {
    curl_easy_cleanup(curl_resource_.easy_handle);
    curl_resource_.easy_handle = nullptr;
  }

  if (curl_resource_.headers_chunk != nullptr)
  {
    curl_slist_free_all(curl_resource_.headers_chunk);
    curl_resource_.headers_chunk = nullptr;
  }
}

CURLcode HttpOperation::Setup()
{
  if (!curl_resource_.easy_handle)
  {
    return CURLE_FAILED_INIT;
  }

  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_VERBOSE, 0);

  // Specify target URL
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_URL, url_.c_str());

  // TODO: support ssl cert verification for https request
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_SSL_VERIFYPEER, 0);  // 1L
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_SSL_VERIFYHOST, 0);  // 2L

  if (curl_resource_.headers_chunk != nullptr)
  {
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_HTTPHEADER, curl_resource_.headers_chunk);
  }

  // TODO: control local port to use
  // curl_easy_setopt(curl, CURLOPT_LOCALPORT, dcf_port);

  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_TIMEOUT_MS, http_conn_timeout_.count());

  // abort if slower than 4kb/sec during 30 seconds
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_LOW_SPEED_TIME, 30L);
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_LOW_SPEED_LIMIT, 4096);
  if (reuse_connection_)
  {
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_FRESH_CONNECT, 0L);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_FORBID_REUSE, 0L);
  }
  else
  {
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_FORBID_REUSE, 1L);
  }

  if (is_raw_response_)
  {
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_HEADER, true);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_WRITEFUNCTION,
                     (void *)&HttpOperation::WriteMemoryCallback);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_WRITEDATA, (void *)this);
  }
  else
  {
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_WRITEFUNCTION,
                     (void *)&HttpOperation::WriteVectorBodyCallback);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_WRITEDATA, (void *)this);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_HEADERFUNCTION,
                     (void *)&HttpOperation::WriteVectorHeaderCallback);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_HEADERDATA, (void *)this);
  }

  // TODO: only two methods supported for now - POST and GET
  if (method_ == opentelemetry::ext::http::client::Method::Post)
  {
    // Request buffer
    const curl_off_t req_size = static_cast<curl_off_t>(request_body_.size());
    // POST
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_POSTFIELDSIZE_LARGE, req_size);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_READFUNCTION,
                     (void *)&HttpOperation::ReadMemoryCallback);
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_READDATA, (void *)this);
  }
  else if (method_ == opentelemetry::ext::http::client::Method::Get)
  {
    // GET
  }
  else
  {
    return CURLE_UNSUPPORTED_PROTOCOL;
  }

#if LIBCURL_VERSION_NUM >= 0x072000
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_XFERINFOFUNCTION,
                   (void *)&HttpOperation::OnProgressCallback);
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_XFERINFODATA, (void *)this);
#else
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PROGRESSFUNCTION,
                   (void *)&HttpOperation::OnProgressCallback);
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PROGRESSDATA, (void *)this);
#endif

#if LIBCURL_VERSION_NUM >= 0x075000
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PREREQFUNCTION,
                   (void *)&HttpOperation::PreRequestCallback);
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PREREQDATA, (void *)this);
#endif

  return CURLE_OK;
}

CURLcode HttpOperation::Send()
{
  // If it is async sending, just return error
  if (async_data_ && async_data_->is_promise_running.load(std::memory_order_acquire))
  {
    return CURLE_FAILED_INIT;
  }

  ReleaseResponse();

  last_curl_result_ = Setup();
  if (last_curl_result_ != CURLE_OK)
  {
    DispatchEvent(opentelemetry::ext::http::client::SessionState::ConnectFailed,
                  curl_easy_strerror(last_curl_result_));
    return last_curl_result_;
  }

  // Perform initial connect, handling the timeout if needed
  // We can not use CURLOPT_CONNECT_ONLY because it will disable the reuse of connections.
  DispatchEvent(opentelemetry::ext::http::client::SessionState::Connecting);
  is_finished_.store(false, std::memory_order_release);
  is_aborted_.store(false, std::memory_order_release);
  is_cleaned_.store(false, std::memory_order_release);

  CURLcode code = curl_easy_perform(curl_resource_.easy_handle);
  PerformCurlMessage(code);
  if (CURLE_OK != code)
  {
    return code;
  }

  return code;
}

CURLcode HttpOperation::SendAsync(Session *session, std::function<void(HttpOperation &)> callback)
{
  if (nullptr == session)
  {
    return CURLE_FAILED_INIT;
  }

  if (async_data_ && async_data_->is_promise_running.load(std::memory_order_acquire))
  {
    return CURLE_FAILED_INIT;
  }

  async_data_.reset(new AsyncData());
  async_data_->is_promise_running.store(false, std::memory_order_release);
  async_data_->session = nullptr;

  ReleaseResponse();

  CURLcode code     = Setup();
  last_curl_result_ = code;
  if (code != CURLE_OK)
  {
    return code;
  }
  curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_PRIVATE, session);

  DispatchEvent(opentelemetry::ext::http::client::SessionState::Connecting);
  is_finished_.store(false, std::memory_order_release);
  is_aborted_.store(false, std::memory_order_release);
  is_cleaned_.store(false, std::memory_order_release);

  async_data_->session = session;
  if (false == async_data_->is_promise_running.exchange(true, std::memory_order_acq_rel))
  {
    async_data_->result_promise = std::promise<CURLcode>();
    async_data_->result_future  = async_data_->result_promise.get_future();
  }
  async_data_->callback = std::move(callback);

  session->GetHttpClient().ScheduleAddSession(session->GetSessionId());
  return code;
}

Headers HttpOperation::GetResponseHeaders()
{
  Headers result;
  if (response_headers_.size() == 0)
    return result;

  std::stringstream ss;
  std::string headers((const char *)&response_headers_[0], response_headers_.size());
  ss.str(headers);

  std::string header;
  while (std::getline(ss, header, '\n'))
  {
    // TODO - Regex below crashes with out-of-memory on CI docker container, so
    // switching to string comparison. Need to debug and revert back.

    /*std::smatch match;
    std::regex http_headers_regex(http_header_regexp);
    if (std::regex_search(header, match, http_headers_regex))
      result.insert(std::pair<nostd::string_view, nostd::string_view>(
          static_cast<nostd::string_view>(match[1]), static_cast<nostd::string_view>(match[2])));
    */
    size_t pos = header.find(": ");
    if (pos != std::string::npos)
      result.insert(
          std::pair<std::string, std::string>(header.substr(0, pos), header.substr(pos + 2)));
  }
  return result;
}

void HttpOperation::ReleaseResponse()
{
  response_headers_.clear();
  response_body_.clear();
  raw_response_.clear();
}

void HttpOperation::Abort()
{
  is_aborted_.store(true, std::memory_order_release);
  if (curl_resource_.easy_handle != nullptr)
  {
    // Enable progress callback to abort from polling thread
    curl_easy_setopt(curl_resource_.easy_handle, CURLOPT_NOPROGRESS, 0L);
    if (async_data_ && nullptr != async_data_->session)
    {
      async_data_->session->GetHttpClient().ScheduleAbortSession(
          async_data_->session->GetSessionId());
    }
  }
}

void HttpOperation::PerformCurlMessage(CURLcode code)
{
  last_curl_result_ = code;
  if (code != CURLE_OK)
  {
    switch (GetSessionState())
    {
      case opentelemetry::ext::http::client::SessionState::Connecting: {
        DispatchEvent(opentelemetry::ext::http::client::SessionState::ConnectFailed,
                      curl_easy_strerror(code));  // couldn't connect - stage 1
        break;
      }
      case opentelemetry::ext::http::client::SessionState::Connected:
      case opentelemetry::ext::http::client::SessionState::Sending: {
        if (GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
        {
          DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
        }

        DispatchEvent(opentelemetry::ext::http::client::SessionState::SendFailed,
                      curl_easy_strerror(code));
      }
      default:
        break;
    }
  }
  else if (curl_resource_.easy_handle != nullptr)
  {
    curl_easy_getinfo(curl_resource_.easy_handle, CURLINFO_RESPONSE_CODE, &response_code_);
  }

  // Transform state
  if (GetSessionState() == opentelemetry::ext::http::client::SessionState::Connecting)
  {
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
  }

  if (GetSessionState() == opentelemetry::ext::http::client::SessionState::Connected)
  {
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);
  }

  if (GetSessionState() == opentelemetry::ext::http::client::SessionState::Sending)
  {
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Response);
  }

  // Cleanup and unbind easy handle from multi handle, and finish callback
  Cleanup();
}

}  // namespace curl
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
