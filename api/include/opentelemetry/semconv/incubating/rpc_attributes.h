/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_attributes-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace rpc
{

/**
  Deprecated, use @code rpc.status_code @endcode attribute instead.

  @deprecated
  {"note": "Replaced by @code rpc.status_code @endcode.", "reason": "renamed", "renamed_to":
  "rpc.status_code"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcConnectRpcErrorCode =
    "rpc.connect_rpc.error_code";

/**
  Deprecated, use @code rpc.request.header @endcode instead.

  @deprecated
  {"note": "Replaced by @code rpc.request.header @endcode.", "reason": "renamed", "renamed_to":
  "rpc.request.header"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcConnectRpcRequestMetadata =
    "rpc.connect_rpc.request.metadata";

/**
  Deprecated, use @code rpc.response.header @endcode and @code rpc.response.trailer @endcode.

  @deprecated
  {"note": "Split to @code rpc.response.header @endcode for metadata sent before the response
  payload and @code rpc.response.trailer @endcode for metadata sent after it.\n", "reason":
  "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcConnectRpcResponseMetadata =
    "rpc.connect_rpc.response.metadata";

/**
  Deprecated, use @code rpc.request.header @endcode instead.

  @deprecated
  {"note": "Replaced by @code rpc.request.header @endcode.", "reason": "renamed", "renamed_to":
  "rpc.request.header"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcGrpcRequestMetadata =
    "rpc.grpc.request.metadata";

/**
  Deprecated, use @code rpc.response.header @endcode and @code rpc.response.trailer @endcode.

  @deprecated
  {"note": "Split to @code rpc.response.header @endcode for metadata sent before the response
  payload and @code rpc.response.trailer @endcode for metadata sent after it.\n", "reason":
  "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcGrpcResponseMetadata =
    "rpc.grpc.response.metadata";

/**
  Deprecated, use string representation on the @code rpc.status_code @endcode attribute instead.

  @deprecated
  {"note": "Use string representation of the gRPC status code on the @code rpc.status_code @endcode
  attribute.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcGrpcStatusCode = "rpc.grpc.status_code";

/**
  Deprecated, use string representation on the @code rpc.status_code @endcode attribute instead.

  @deprecated
  {"note": "Use string representation of the error code on the @code rpc.status_code @endcode
  attribute.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcJsonrpcErrorCode =
    "rpc.jsonrpc.error_code";

/**
  Deprecated, use the span status description when reporting JSON-RPC spans.

  @deprecated
  {"note": "Use the span status description when reporting JSON-RPC spans.", "reason":
  "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcJsonrpcErrorMessage =
    "rpc.jsonrpc.error_message";

/**
  Deprecated, use @code jsonrpc.request.id @endcode instead.

  @deprecated
  {"note": "Replaced by @code jsonrpc.request.id @endcode.", "reason": "renamed", "renamed_to":
  "jsonrpc.request.id"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcJsonrpcRequestId =
    "rpc.jsonrpc.request_id";

/**
  Deprecated, use @code jsonrpc.protocol.version @endcode instead.

  @deprecated
  {"note": "Replaced by @code jsonrpc.protocol.version @endcode.", "reason": "renamed",
  "renamed_to": "jsonrpc.protocol.version"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcJsonrpcVersion = "rpc.jsonrpc.version";

/**
  Compressed size of the message in bytes.

  @deprecated
  {"note": "Deprecated, no replacement at this time.", "reason": "obsoleted"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcMessageCompressedSize =
    "rpc.message.compressed_size";

/**
  MUST be calculated as two different counters starting from @code 1 @endcode one for sent messages
  and one for received message.

  @deprecated
  {"note": "Deprecated, no replacement at this time.", "reason": "obsoleted"}
  <p>
  This way we guarantee that the values will be consistent between different implementations.
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcMessageId = "rpc.message.id";

/**
  Whether this is a received or sent message.

  @deprecated
  {"note": "Deprecated, no replacement at this time.", "reason": "obsoleted"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcMessageType = "rpc.message.type";

/**
  Uncompressed size of the message in bytes.

  @deprecated
  {"note": "Deprecated, no replacement at this time.", "reason": "obsoleted"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcMessageUncompressedSize =
    "rpc.message.uncompressed_size";

/**
  The fully-qualified logical name of the method from the RPC interface perspective.
  <p>
  The method name MAY have unbounded cardinality in edge or error cases.
  <p>
  Some RPC frameworks or libraries provide a fixed set of recognized methods
  for client stubs and server implementations. Instrumentations for such
  frameworks MUST set this attribute to the original method name only
  when the method is recognized by the framework or library.
  <p>
  When the method is not recognized, for example, when the server receives
  a request for a method that is not predefined on the server, or when
  instrumentation is not able to reliably detect if the method is predefined,
  the attribute MUST be set to @code _OTHER @endcode.
  <p>
  If the RPC instrumentation could end up converting valid RPC methods to
  @code _OTHER @endcode, then it SHOULD provide a way to configure the list of recognized
  RPC methods.
  <p>
  The @code rpc.method @endcode can be different from the name of any implementing
  method/function.
  The @code code.function.name @endcode attribute may be used to record the fully-qualified
  method actually executing the call on the server side, or the
  RPC client stub method on the client side.
 */
static constexpr const char *kRpcMethod = "rpc.method";

/**
  The original name of the method used by the client.
 */
static constexpr const char *kRpcMethodOriginal = "rpc.method_original";

/**
  RPC request headers or metadata, @code <key> @endcode being the normalized header name
  (lowercase), the value being the header values. <p> Instrumentations SHOULD require an explicit
  configuration of which headers are to be captured. Including all request headers can be a security
  risk - explicit configuration helps avoid leaking sensitive information. <p> For example, a header
  @code my-custom-key @endcode with value @code ["1.2.3.4", "1.2.3.5"] @endcode SHOULD be recorded
  as the @code rpc.request.header.my-custom-key @endcode attribute with value @code ["1.2.3.4",
  "1.2.3.5"] @endcode <p> The attribute value MUST consist of either multiple header values as an
  array of strings or a single-item array containing a possibly comma-concatenated string, depending
  on the way the RPC library provides access to headers. <p> Values that the RPC system exposes as
  binary data, or that are otherwise not valid UTF-8 strings, SHOULD be base64-encoded when
  recorded.
 */
static constexpr const char *kRpcRequestHeader = "rpc.request.header";

/**
  Deprecated, use @code rpc.request.header @endcode instead.

  @deprecated
  {"note": "Replaced by @code rpc.request.header @endcode.", "reason": "renamed", "renamed_to":
  "rpc.request.header"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcRequestMetadata = "rpc.request.metadata";

/**
  RPC response headers or metadata sent before the response payload, @code <key> @endcode being the
  normalized header name (lowercase), the value being the header values. <p> Instrumentations SHOULD
  require an explicit configuration of which headers are to be captured. Including all response
  headers can be a security risk - explicit configuration helps avoid leaking sensitive information.
  <p>
  For example, a header @code my-custom-key @endcode with value @code ["attribute_value"] @endcode
  SHOULD be recorded as the @code rpc.response.header.my-custom-key @endcode attribute with value
  @code ["attribute_value"] @endcode <p> The attribute value MUST consist of either multiple header
  values as an array of strings or a single-item array containing a possibly comma-concatenated
  string, depending on the way the RPC library provides access to headers. <p> Values that the RPC
  system exposes as binary data, or that are otherwise not valid UTF-8 strings, SHOULD be
  base64-encoded when recorded.
 */
static constexpr const char *kRpcResponseHeader = "rpc.response.header";

/**
  Deprecated, use @code rpc.response.header @endcode and @code rpc.response.trailer @endcode.

  @deprecated
  {"note": "Split to @code rpc.response.header @endcode for metadata sent before the response
  payload and @code rpc.response.trailer @endcode for metadata sent after it.\n", "reason":
  "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcResponseMetadata =
    "rpc.response.metadata";

/**
  Deprecated, use @code rpc.status_code @endcode instead.

  @deprecated
  {"note": "Replaced by @code rpc.status_code @endcode.", "reason": "renamed", "renamed_to":
  "rpc.status_code"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcResponseStatusCode =
    "rpc.response.status_code";

/**
  RPC response trailers or metadata sent after the response payload, @code <key> @endcode being the
  normalized trailer name (lowercase), the value being the trailer values. <p> Instrumentations
  SHOULD require an explicit configuration of which trailers are to be captured. Including all
  response trailers can be a security risk - explicit configuration helps avoid leaking sensitive
  information. <p> For example, a trailer @code my-custom-key @endcode with value @code
  ["attribute_value"] @endcode SHOULD be recorded as the @code rpc.response.trailer.my-custom-key
  @endcode attribute with value @code ["attribute_value"] @endcode <p> The attribute value MUST
  consist of either multiple trailer values as an array of strings or a single-item array containing
  a possibly comma-concatenated string, depending on the way the RPC library provides access to
  trailers. <p> Values that the RPC system exposes as binary data, or that are otherwise not valid
  UTF-8 strings, SHOULD be base64-encoded when recorded.
 */
static constexpr const char *kRpcResponseTrailer = "rpc.response.trailer";

/**
  Deprecated, use fully-qualified @code rpc.method @endcode instead.

  @deprecated
  {"note": "Value should be included in @code rpc.method @endcode which is expected to be a
  fully-qualified name.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcService = "rpc.service";

/**
  Status code of the RPC returned by the RPC server or generated by the client
  <p>
  Usually it represents an error code, but may also represent partial success, warning, or
  differentiate between various types of successful outcomes. The value MAY use a standardized
  string representation of a numeric status code, if one exists, instead of the numeric code. The
  value MAY be a synthetic status provided by the client library rather than returned by the remote
  service. Semantic conventions for individual RPC frameworks SHOULD document what @code
  rpc.status_code @endcode means in the context of that system and which values are considered to
  represent errors.
 */
static constexpr const char *kRpcStatusCode = "rpc.status_code";

/**
  Deprecated, use @code rpc.system.name @endcode attribute instead.

  @deprecated
  {"note": "Replaced by @code rpc.system.name @endcode.", "reason": "renamed", "renamed_to":
  "rpc.system.name"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kRpcSystem = "rpc.system";

/**
  The Remote Procedure Call (RPC) system.
  <p>
  The client and server RPC systems may differ for the same RPC interaction. For example, a client
  may use Apache Dubbo or Connect RPC to communicate with a server that uses gRPC since both
  protocols provide compatibility with gRPC.
 */
static constexpr const char *kRpcSystemName = "rpc.system.name";

namespace RpcConnectRpcErrorCodeValues
{

static constexpr const char *kCancelled = "cancelled";

static constexpr const char *kUnknown = "unknown";

static constexpr const char *kInvalidArgument = "invalid_argument";

static constexpr const char *kDeadlineExceeded = "deadline_exceeded";

static constexpr const char *kNotFound = "not_found";

static constexpr const char *kAlreadyExists = "already_exists";

static constexpr const char *kPermissionDenied = "permission_denied";

static constexpr const char *kResourceExhausted = "resource_exhausted";

static constexpr const char *kFailedPrecondition = "failed_precondition";

static constexpr const char *kAborted = "aborted";

static constexpr const char *kOutOfRange = "out_of_range";

static constexpr const char *kUnimplemented = "unimplemented";

static constexpr const char *kInternal = "internal";

static constexpr const char *kUnavailable = "unavailable";

static constexpr const char *kDataLoss = "data_loss";

static constexpr const char *kUnauthenticated = "unauthenticated";

}  // namespace RpcConnectRpcErrorCodeValues

namespace RpcGrpcStatusCodeValues
{
/**
  OK
 */
static constexpr int kOk = 0;

/**
  CANCELLED
 */
static constexpr int kCancelled = 1;

/**
  UNKNOWN
 */
static constexpr int kUnknown = 2;

/**
  INVALID_ARGUMENT
 */
static constexpr int kInvalidArgument = 3;

/**
  DEADLINE_EXCEEDED
 */
static constexpr int kDeadlineExceeded = 4;

/**
  NOT_FOUND
 */
static constexpr int kNotFound = 5;

/**
  ALREADY_EXISTS
 */
static constexpr int kAlreadyExists = 6;

/**
  PERMISSION_DENIED
 */
static constexpr int kPermissionDenied = 7;

/**
  RESOURCE_EXHAUSTED
 */
static constexpr int kResourceExhausted = 8;

/**
  FAILED_PRECONDITION
 */
static constexpr int kFailedPrecondition = 9;

/**
  ABORTED
 */
static constexpr int kAborted = 10;

/**
  OUT_OF_RANGE
 */
static constexpr int kOutOfRange = 11;

/**
  UNIMPLEMENTED
 */
static constexpr int kUnimplemented = 12;

/**
  INTERNAL
 */
static constexpr int kInternal = 13;

/**
  UNAVAILABLE
 */
static constexpr int kUnavailable = 14;

/**
  DATA_LOSS
 */
static constexpr int kDataLoss = 15;

/**
  UNAUTHENTICATED
 */
static constexpr int kUnauthenticated = 16;

}  // namespace RpcGrpcStatusCodeValues

namespace RpcMessageTypeValues
{

static constexpr const char *kSent = "SENT";

static constexpr const char *kReceived = "RECEIVED";

}  // namespace RpcMessageTypeValues

namespace RpcSystemValues
{
/**
  gRPC
 */
static constexpr const char *kGrpc = "grpc";

/**
  Java RMI
 */
static constexpr const char *kJavaRmi = "java_rmi";

/**
  .NET WCF
 */
static constexpr const char *kDotnetWcf = "dotnet_wcf";

/**
  Apache Dubbo
 */
static constexpr const char *kApacheDubbo = "apache_dubbo";

/**
  Connect RPC
 */
static constexpr const char *kConnectRpc = "connect_rpc";

/**
  <a href="https://datatracker.ietf.org/doc/html/rfc5531">ONC RPC (Sun RPC)</a>
 */
static constexpr const char *kOncRpc = "onc_rpc";

/**
  JSON-RPC
 */
static constexpr const char *kJsonrpc = "jsonrpc";

}  // namespace RpcSystemValues

namespace RpcSystemNameValues
{
/**
  <a href="https://grpc.io/">gRPC</a>
 */
static constexpr const char *kGrpc = "grpc";

/**
  <a href="https://dubbo.apache.org/">Apache Dubbo</a>
 */
static constexpr const char *kDubbo = "dubbo";

/**
  <a href="https://connectrpc.com/">Connect RPC</a>
 */
static constexpr const char *kConnectrpc = "connectrpc";

/**
  <a href="https://www.jsonrpc.org/">JSON-RPC</a>
 */
static constexpr const char *kJsonrpc = "jsonrpc";

}  // namespace RpcSystemNameValues

}  // namespace rpc
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
