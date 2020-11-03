#pragma once

#include "opentelemetry/sdk/common/http_client.h"

#include <curl/curl.h>
#include <future>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#  include <io.h>
#else
#  include <unistd.h>
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporters
{
namespace common
{
namespace http
{
namespace curl
{
namespace http_sdk                     = opentelemetry::sdk::common::http;
const size_t default_http_conn_timeout = 5000;  // ms
const std::string http_status_regexp   = "HTTP\\/\\d\\.\\d (\\d+)\\ .*";
const std::string http_header_regexp   = "(.*)\\: (.*)\\n*";

struct curl_ci
{
  bool operator()(const nostd::string_view &s1, const nostd::string_view &s2) const
  {
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(),
        [](char c1, char c2) { return ::tolower(c1) < ::tolower(c2); });
  }
};
using Headers = std::multimap<nostd::string_view, nostd::string_view, curl_ci>;

class HttpOperation
{
public:
  void DispatchEvent(http_sdk::SessionState type, std::string reason = "")
  {
    if (callback_ != nullptr)
    {
      callback_->OnEvent(type, reason);
    }
  }

  std::atomic<bool> is_aborted_;   // Set to 'true' when async callback is aborted
  std::atomic<bool> is_finished_;  // Set to 'true' when async callback is finished.

  /**
   * Create local CURL instance for url and body
   *
   * @param url
   * @param body
   * @param httpConnTimeout   HTTP connection timeout in seconds
   * @param httpReadTimeout   HTTP read timeout in seconds
   */
  HttpOperation(http_sdk::Method method,
                std::string url,
                http_sdk::EventHandler *callback,
                // Default empty headers and empty request body
                const Headers &request_headers     = Headers(),
                const http_sdk::Body &request_body = http_sdk::Body(),
                // Default connectivity and response size options
                bool raw_response        = false,
                size_t http_conn_timeout = default_http_conn_timeout)
      :

        //
        method_(method),
        url_(url),
        callback_(callback),

        // Local vars
        request_headers_(request_headers),
        request_body_(request_body),
        // Optional connection params
        raw_response_(raw_response),
        http_conn_timeout_(http_conn_timeout),
        // Result
        res_(CURLE_OK),
        sockfd_(0),
        is_aborted_(false),
        is_finished_(false),
        nread_(0)
  {
    response_.memory = nullptr;
    response_.size   = 0;

    /* get a curl handle */
    curl_ = curl_easy_init();
    if (!curl_)
    {
      res_ = CURLE_FAILED_INIT;
      DispatchEvent(http_sdk::SessionState::CreateFailed);
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
      std::string header = static_cast<std::string>(kv.first);
      header += ": ";
      header += static_cast<std::string>(kv.second);
      headers_chunk_ = curl_slist_append(headers_chunk_, header.c_str());
    }

    if (headers_chunk_ != nullptr)
    {
      curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_chunk_);
    }

    DispatchEvent(http_sdk::SessionState::Created);
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
    // DispatchEvent(http_sdk::SessionState::Destroy);
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
      DispatchEvent(http_sdk::SessionState::SendFailed);
      return res_;
    }

    // TODO: control local port to use
    // curl_easy_setopt(curl, CURLOPT_LOCALPORT, dcf_port);

    // Perform initial connect, handling the timeout if needed

    curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 1L);
    DispatchEvent(http_sdk::SessionState::Connecting);
    res_ = curl_easy_perform(curl_);

    if (CURLE_OK != res_)
    {
      DispatchEvent(http_sdk::SessionState::ConnectFailed,
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
      DispatchEvent(http_sdk::SessionState::ConnectFailed,
                    curl_easy_strerror(res_));  // couldn't connect - stage 2
      return res_;
    }

    /* wait for the socket to become ready for sending */
    sockfd_ = sockextr;
    if (!WaitOnSocket(sockfd_, 0, http_conn_timeout_) || is_aborted_)
    {
      res_ = CURLE_OPERATION_TIMEDOUT;
      DispatchEvent(
          http_sdk::SessionState::ConnectFailed,
          " Is aborted: " + std::to_string(is_aborted_.load()));  // couldn't connect - stage 3
      return res_;
    }

    DispatchEvent(http_sdk::SessionState::Connected);
    // once connection is there - switch back to easy perform for HTTP post
    curl_easy_setopt(curl_, CURLOPT_CONNECT_ONLY, 0);

    // send all data to our callback function
    if (raw_response_)
    {
      curl_easy_setopt(curl_, CURLOPT_HEADER, true);
      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, (void *)&WriteMemoryCallback);
      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&response_);
    }
    else
    {
      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, (void *)&WriteVectorCallback);
      curl_easy_setopt(curl_, CURLOPT_HEADERDATA, (void *)&resp_headers_);
      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)&resp_body_);
    }

    // TODO: only two methods supported for now - POST and GET
    if (method_ == http_sdk::Method::Post)
    {
      // POST
      curl_easy_setopt(curl_, CURLOPT_POST, true);
      curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, (const char *)request);
      curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, req_size);
    }
    else if (method_ == http_sdk::Method::Get)
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
    DispatchEvent(http_sdk::SessionState::Sending);

    res_ = curl_easy_perform(curl_);
    if (CURLE_OK != res_)
    {
      DispatchEvent(http_sdk::SessionState::SendFailed, curl_easy_strerror(res_));
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
    DispatchEvent(http_sdk::SessionState::Response);

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

  /**
   * Get HTTP response code. This function returns CURL error code if HTTP response code is invalid.
   */
  long GetResponseCode() { return res_; }

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
      std::smatch match;
      std::regex http_headers_regex(http_header_regexp);
      if (std::regex_search(header, match, http_headers_regex))
        result.insert(std::pair<nostd::string_view, nostd::string_view>(match[1], match[2]));
      // result[match.str(1)] = match[2];    // Key: value
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
  std::vector<uint8_t> GetRawResponse()
  {
    std::vector<uint8_t> result;
    if ((response_.memory != nullptr) && (response_.size != 0))
      result.insert(result.end(), (const char *)response_.memory,
                    ((const char *)response_.memory) + response_.size);
    return result;
  }

  /**
   * Release memory allocated for response
   */
  void ReleaseResponse()
  {
    if (response_.memory != nullptr)
    {
      free(response_.memory);
      response_.memory = nullptr;
      response_.size   = 0;
    }
    resp_headers_.clear();
    resp_body_.clear();
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
        ::close(sockfd_);
        sockfd_ = 0;
      }
    }
  }

  CURL *GetHandle() { return curl_; }

protected:
  const bool raw_response_;         // Do not split response headers from response body
  const size_t http_conn_timeout_;  // Timeout for connect.  Default: 5000ms

  CURL *curl_;    // Local curl instance
  CURLcode res_;  // Curl result OR HTTP status code if successful

  http_sdk::EventHandler *callback_ = nullptr;

  // Request values
  http_sdk::Method method_;
  std::string url_;
  const Headers &request_headers_;
  const http_sdk::Body &request_body_;
  struct curl_slist *headers_chunk_ = nullptr;

  // Processed response headers and body
  std::vector<uint8_t> resp_headers_;
  std::vector<uint8_t> resp_body_;

  // Socket parameters
  curl_socket_t sockfd_;

  // long sockextr_   = 0;

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

  // Raw response buffer
  struct MemoryStruct
  {
    char *memory;
    size_t size;
  } response_;

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
    size_t realsize          = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char *)(realloc(mem->memory, mem->size + realsize + 1));
    if (mem->memory == NULL)
    {
      /* out of memory! */
      return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
  }

  /**
   * C++ STL std::string allocator
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
}  // namespace http
}  // namespace common
}  // namespace exporters
OPENTELEMETRY_END_NAMESPACE
