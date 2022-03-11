// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "http_client_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/version.h"

#include <future>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#  include <io.h>
#  include <winsock2.h>
#else
#  include <unistd.h>
#endif
#include <curl/curl.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
namespace curl
{
const std::chrono::milliseconds default_http_conn_timeout(5000);  // ms
const std::string http_status_regexp = "HTTP\\/\\d\\.\\d (\\d+)\\ .*";
const std::string http_header_regexp = "(.*)\\: (.*)\\n*";

enum class RequestMode
{
  Sync,
  Async
};

class HttpOperation
{
public:
  void DispatchEvent(opentelemetry::ext::http::client::SessionState type, std::string reason = "")
  {
    if (request_mode_ == RequestMode::Async && callback_ != nullptr)
    {
      callback_->OnEvent(type, reason);
    }
    else
    {
      session_state_ = type;
    }
  }

  std::atomic<bool> is_aborted_;   // Set to 'true' when async callback is aborted
  std::atomic<bool> is_finished_;  // Set to 'true' when async callback is finished.

  /**
   * Create local CURL instance for url and body
   * @param method // HTTP Method
   * @param url    // HTTP URL
   * @param callback
   * @param request_mode // sync or async
   * @param request  Request Headers
   * @param body  Reques Body
   * @param raw_response whether to parse the response
   * @param httpConnTimeout   HTTP connection timeout in seconds
   */
  HttpOperation(opentelemetry::ext::http::client::Method method,
                std::string url,
                opentelemetry::ext::http::client::EventHandler *callback,
                RequestMode request_mode = RequestMode::Async,
                // Default empty headers and empty request body
                const opentelemetry::ext::http::client::Headers &request_headers =
                    opentelemetry::ext::http::client::Headers(),
                const opentelemetry::ext::http::client::Body &request_body =
                    opentelemetry::ext::http::client::Body(),
                // Default connectivity and response size options
                bool is_raw_response                        = false,
                std::chrono::milliseconds http_conn_timeout = default_http_conn_timeout)
      : is_aborted_(false),
        is_finished_(false),
        // Optional connection params
        is_raw_response_(is_raw_response),
        http_conn_timeout_(http_conn_timeout),
        request_mode_(request_mode),
        curl_(nullptr),
        // Result
        res_(CURLE_OK),
        callback_(callback),
        method_(method),
        url_(url),
        // Local vars
        request_headers_(request_headers),
        request_body_(request_body),
        sockfd_(0),
        nread_(0)
  {
    /* get a curl handle */
    curl_ = curl_easy_init();
    if (!curl_)
    {
      res_ = CURLE_FAILED_INIT;
      DispatchEvent(opentelemetry::ext::http::client::SessionState::CreateFailed);
      return;
    }

    curl_easy_setopt(curl_, CURLOPT_VERBOSE, 0);

    // Specify target URL
    curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());

    // TODO: support ssl cert verification for https request
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0);  // 1L
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0);  // 2L

    // Specify our custom headers
    for (auto &kv : this->request_headers_)
    {
      std::string header = std::string(kv.first);
      header += ": ";
      header += std::string(kv.second);
      headers_chunk_ = curl_slist_append(headers_chunk_, header.c_str());
    }

    if (headers_chunk_ != nullptr)
    {
      curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_chunk_);
    }

    DispatchEvent(opentelemetry::ext::http::client::SessionState::Created);
  }

  /**
   * Destroy CURL instance
   */
  virtual ~HttpOperation()
  {
    // Given the request has not been aborted we should wait for completion here
    // This guarantees the lifetime of this request.
    if (result_.valid())
    {
      result_.wait();
    }
    // TBD - Need to be uncomment. This will callback instance is deleted.
    // DispatchEvent(opentelemetry::ext::http::client::SessionState::Destroy);
    res_ = CURLE_OK;
    curl_easy_cleanup(curl_);
    curl_slist_free_all(headers_chunk_);
    ReleaseResponse();
  }

  /**
   * Finish CURL instance
   */
  virtual void Finish()
  {
    if (result_.valid() && !is_finished_)
    {
      result_.wait();
      is_finished_ = true;
    }
  }

  /**
   * Send request synchronously
   */
  long Send()
  {
    ReleaseResponse();
    // Request buffer
    const void *request   = (request_body_.empty()) ? NULL : &request_body_[0];
    const size_t req_size = request_body_.size();
    if (!curl_)
    {
      res_ = CURLE_FAILED_INIT;
      DispatchEvent(opentelemetry::ext::http::client::SessionState::SendFailed);
      return res_;
    }

    // TODO: control local port to use
    // curl_easy_setopt(curl, CURLOPT_LOCALPORT, dcf_port);

    // Perform initial connect, handling the timeout if needed
    curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 1L);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, http_conn_timeout_.count());
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Connecting);
    res_ = curl_easy_perform(curl_);
    if (CURLE_OK != res_)
    {
      DispatchEvent(opentelemetry::ext::http::client::SessionState::ConnectFailed,
                    curl_easy_strerror(res_));  // couldn't connect - stage 1
      return res_;
    }

    /* Extract the socket from the curl handle - we'll need it for waiting.
     * Note that this API takes a pointer to a 'long' while we use
     * curl_socket_t for sockets otherwise.
     */
    long sockextr = 0;
    res_          = curl_easy_getinfo(curl_, CURLINFO_LASTSOCKET, &sockextr);

    if (CURLE_OK != res_)
    {
      DispatchEvent(opentelemetry::ext::http::client::SessionState::ConnectFailed,
                    curl_easy_strerror(res_));  // couldn't connect - stage 2
      return res_;
    }

    /* wait for the socket to become ready for sending */
    sockfd_ = sockextr;
    if (!WaitOnSocket(sockfd_, 0, static_cast<long>(http_conn_timeout_.count())) || is_aborted_)
    {
      res_ = CURLE_OPERATION_TIMEDOUT;
      DispatchEvent(
          opentelemetry::ext::http::client::SessionState::ConnectFailed,
          " Is aborted: " + std::to_string(is_aborted_.load()));  // couldn't connect - stage 3
      return res_;
    }

    DispatchEvent(opentelemetry::ext::http::client::SessionState::Connected);
    // once connection is there - switch back to easy perform for HTTP post
    curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 0);

    // send all data to our callback function
    if (is_raw_response_)
    {
      curl_easy_setopt(curl_, CURLOPT_HEADER, true);
      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, (void *)&WriteMemoryCallback);
      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&raw_response_);
    }
    else
    {
      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, (void *)&WriteVectorCallback);
      curl_easy_setopt(curl_, CURLOPT_HEADERDATA, (void *)&resp_headers_);
      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&resp_body_);
    }

    // TODO: only two methods supported for now - POST and GET
    if (method_ == opentelemetry::ext::http::client::Method::Post)
    {
      // POST
      curl_easy_setopt(curl_, CURLOPT_POST, true);
      curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, (const char *)request);
      curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, req_size);
    }
    else if (method_ == opentelemetry::ext::http::client::Method::Get)
    {
      // GET
    }
    else
    {
      res_ = CURLE_UNSUPPORTED_PROTOCOL;
      return res_;
    }

    // abort if slower than 4kb/sec during 30 seconds
    curl_easy_setopt(curl_, CURLOPT_LOW_SPEED_TIME, 30L);
    curl_easy_setopt(curl_, CURLOPT_LOW_SPEED_LIMIT, 4096);
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Sending);

    res_ = curl_easy_perform(curl_);
    if (CURLE_OK != res_)
    {
      DispatchEvent(opentelemetry::ext::http::client::SessionState::SendFailed,
                    curl_easy_strerror(res_));
      return res_;
    }

    /* Code snippet to parse raw HTTP response. This might come in handy
     * if we ever consider to handle the raw upload instead of curl_easy_perform
   ...
   std::string resp((const char *)response);
   std::regex http_status_regex(HTTP_STATUS_REGEXP);
   std::smatch match;
   if(std::regex_search(resp, match, http_status_regex))
     http_code = std::stol(match[1]);
   ...
     */

    /* libcurl is nice enough to parse the http response code itself: */
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &res_);
    // We got some response from server. Dump the contents.
    DispatchEvent(opentelemetry::ext::http::client::SessionState::Response);

    // This function returns:
    // - on success: HTTP status code.
    // - on failure: CURL error code.
    // The two sets of enums (CURLE, HTTP codes) - do not intersect, so we collapse them in one set.
    return res_;
  }

  std::future<long> &SendAsync(std::function<void(HttpOperation &)> callback = nullptr)
  {
    result_ = std::async(std::launch::async, [this, callback] {
      long result = Send();
      if (callback != nullptr)
      {
        callback(*this);
      }
      return result;
    });
    return result_;
  }

  void SendSync() { Send(); }

  /**
   * Get HTTP response code. This function returns CURL error code if HTTP response code is invalid.
   */
  uint16_t GetResponseCode() { return res_; }

  /**
   * Get last session state.
   */
  opentelemetry::ext::http::client::SessionState GetSessionState() { return session_state_; }

  /**
   * Get whether or not response was programmatically aborted
   */
  bool WasAborted() { return is_aborted_.load(); }

  /**
   * Return a copy of resposne headers
   *
   * @return
   */
  Headers GetResponseHeaders()
  {
    Headers result;
    if (resp_headers_.size() == 0)
      return result;

    std::stringstream ss;
    std::string headers((const char *)&resp_headers_[0], resp_headers_.size());
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

  /**
   * Return a copy of response body
   *
   * @return
   */
  std::vector<uint8_t> GetResponseBody() { return resp_body_; }

  /**
   * Return a raw copy of response headers+body
   *
   * @return
   */
  std::vector<uint8_t> GetRawResponse() { return raw_response_; }

  /**
   * Release memory allocated for response
   */
  void ReleaseResponse()
  {
    resp_headers_.clear();
    resp_body_.clear();
    raw_response_.clear();
  }

  /**
   * Abort request in connecting or reading state.
   */
  void Abort()
  {
    is_aborted_ = true;
    if (curl_ != nullptr)
    {
      // Simply close the socket - connection reset by peer
      if (sockfd_)
      {
#if defined(_WIN32)
        ::closesocket(sockfd_);
#else
        ::close(sockfd_);
#endif
        sockfd_ = 0;
      }
    }
  }

  CURL *GetHandle() { return curl_; }

protected:
  const bool is_raw_response_;  // Do not split response headers from response body
  const std::chrono::milliseconds http_conn_timeout_;  // Timeout for connect.  Default: 5000ms
  RequestMode request_mode_;

  CURL *curl_;    // Local curl instance
  CURLcode res_;  // Curl result OR HTTP status code if successful

  opentelemetry::ext::http::client::EventHandler *callback_;

  // Request values
  opentelemetry::ext::http::client::Method method_;
  std::string url_;
  const Headers &request_headers_;
  const opentelemetry::ext::http::client::Body &request_body_;
  struct curl_slist *headers_chunk_ = nullptr;
  opentelemetry::ext::http::client::SessionState session_state_;

  // Processed response headers and body
  std::vector<uint8_t> resp_headers_;
  std::vector<uint8_t> resp_body_;
  std::vector<uint8_t> raw_response_;

  // Socket parameters
  curl_socket_t sockfd_;

  curl_off_t nread_;
  size_t sendlen_ = 0;  // # bytes sent by client
  size_t acklen_  = 0;  // # bytes ack by server

  std::future<long> result_;

  /**
   * Helper routine to wait for data on socket
   *
   * @param sockfd
   * @param for_recv
   * @param timeout_ms
   * @return
   */
  static int WaitOnSocket(curl_socket_t sockfd, int for_recv, long timeout_ms)
  {
    struct timeval tv;
    fd_set infd, outfd, errfd;
    int res;

    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    FD_ZERO(&infd);
    FD_ZERO(&outfd);
    FD_ZERO(&errfd);

    FD_SET(sockfd, &errfd); /* always check for error */

    if (for_recv)
    {
      FD_SET(sockfd, &infd);
    }
    else
    {
      FD_SET(sockfd, &outfd);
    }

    /* select() returns the number of signalled sockets or -1 */
    res = select((int)sockfd + 1, &infd, &outfd, &errfd, &tv);
    return res;
  }

  /**
   * Old-school memory allocator
   *
   * @param contents
   * @param size
   * @param nmemb
   * @param userp
   * @return
   */
  static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
  {
    std::vector<char> *buf = static_cast<std::vector<char> *>(userp);
    buf->insert(buf->end(), static_cast<char *>(contents),
                static_cast<char *>(contents) + (size * nmemb));
    return size * nmemb;
  }

  /**
   * C++ STL std::vector allocator
   *
   * @param ptr
   * @param size
   * @param nmemb
   * @param data
   * @return
   */
  static size_t WriteVectorCallback(void *ptr,
                                    size_t size,
                                    size_t nmemb,
                                    std::vector<uint8_t> *data)
  {
    if (data != nullptr)
    {
      const unsigned char *begin = (unsigned char *)(ptr);
      const unsigned char *end   = begin + size * nmemb;
      data->insert(data->end(), begin, end);
    }
    return size * nmemb;
  }
};
}  // namespace curl
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
