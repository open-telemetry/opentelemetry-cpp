// Copyright The OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "opentelemetry/nostd/string_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace http
{

enum class HttpResult : uint8_t
{
  // Response has been received successfully from target server.
  HttpResult_OK = 0,

  // Request has been aborted by the caller. The server might or
  // might not have already received or processed the request.
  HttpResult_Aborted = 1,

  // Local conditions have prevented the request from being sent
  // (invalid request parameters, out of memory, internal error etc.)
  HttpResult_LocalFailure = 2,

  // Network conditions somewhere between the local machine and
  // the target server have caused the request to fail
  // (connection failed, connection dropped abruptly etc.).
  HttpResult_NetworkFailure = 3

};

// The HttpHeaders class contains a set of HTTP headers.
class HttpHeaders
{
public:
  // Inserts a name/value pair, and removes elements with the same name.
  virtual void set(nostd::string_view const &name, nostd::string_view const &value) = 0;

  // Inserts a name/value pair
  virtual void add(nostd::string_view const &name, nostd::string_view const &value) = 0;

  // Gets a string value given a name
  // Returns:
  //      If there are multiple headers with same name, it returns any one of the value
  //      (implementation defined) Empty string if there is no header with speicfied name..
  virtual nostd::string_view const &get(nostd::string_view const &name) const = 0;

  // Tests whether the headers contain the specified name.
  virtual bool contains(nostd::string_view const &name) const = 0;
};

// The HttpRequest class represents a Request object.
// Individual HTTP client implementations can implement the request object in
// the most efficient way. Either fill the request first, and then issue
// the underlying request, or create the real request immediately, and then forward
// the methods and set the individual parameters directly one by one.
class HttpRequest
{
public:
  // Gets the request ID.
  virtual const nostd::string_view &GetId() const = 0;

  // Sets the request Method
  virtual void SetMethod(nostd::string_view const &method) = 0;

  // Gets the request URI
  virtual void SetUrl(nostd::string_view const &url) = 0;

  // Gets the request Headers
  virtual HttpHeaders *GetHeaders() const = 0;

  // Sets the request body
  virtual void SetBody(const uint8_t *const body, const size_t len) = 0;

  // Gets the request body
  virtual void GetBody(uint8_t *body, size_t &len) = 0;
};

// The HttpResponse class represents a Response object.
// Individual HTTP client implementations can implement the response object
// in the most efficient way. Either copy all of the underlying data to a new
// structure, and provide it to the callback; or keep the real
// response data around, forward the methods, and retrieve the individual
// values directly one by one.
class HttpResponse
{
public:
  // Gets the response ID.
  virtual const nostd::string_view &GetId() = 0;

  // Get the response result code.
  virtual HttpResult GetResult() = 0;

  // Gets the response status code.
  virtual unsigned GetStatusCode() = 0;

  // Gets the response headers.
  virtual HttpHeaders *GetHeaders() = 0;

  // Gets the response body.
  virtual void GetBody(uint8_t *body, size_t &len) = 0;
};

// The HttpResponseCallback class receives HTTP client responses
class HttpResponseCallback
{
public:
  // Called when an HTTP request completes.
  // The passed response object contains details about the exact way the
  // request finished (HTTP status code, headers, content, error codes
  // etc.). The ownership of the response object is transferred to the
  // callback object. It can store it for later if necessary. Finally, it
  // must be deleted using its virtual destructor.
  virtual void OnHttpResponse(HttpResponse *response) = 0;
};

// The HttpClient class is the interface for HTTP client implementations.
class HttpClient
{
public:
  // Creates an empty HTTP request object.
  // The created request object has only its ID prepopulated. Other fields
  // must be set by the caller. The request object can then be sent
  // using SendRequestAsync(). If you are not going to use the request object
  // then you can delete it safely using its virtual destructor.
  virtual HttpRequest *CreateRequest() = 0;

  // Begins an HTTP request.
  // The method takes ownership of the passed request, and can destroy it before
  // returning to the caller. Do not access the request object in any
  // way after this invocation, and do not delete it.
  // The callback object is always called, even if the request is
  // cancelled, or if an error occurs immediately during sending. In the
  // latter case, the OnHttpResponse() callback is called before this
  // method returns. You must keep the callback object alive until its
  // OnHttpResponse() callback is called.
  virtual void SendRequest(HttpRequest *request, HttpResponseCallback *callback) = 0;

  // Cancels an HTTP request.
  // The caller must provide a string ID returned earlier by request->GetId().
  // The request is cancelled asynchronously. The caller must still
  // wait for the relevant OnHttpResponse() callback (it can just come
  // earlier with some "aborted" error status).
  virtual void CancelRequest(nostd::string_view const &id) = 0;

  virtual void CancelAllRequestsSync() {}
};

}  // namespace http
OPENTELEMETRY_END_NAMESPACE
