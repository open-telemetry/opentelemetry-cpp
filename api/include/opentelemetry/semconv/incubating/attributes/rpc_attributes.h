

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
 * The <a href="https://connect.build/docs/protocol/#error-codes">error codes</a> of the Connect
 * request. Error codes are always string values.
 */
static const char *kRpcConnectRpcErrorCode = "rpc.connect_rpc.error_code";

/**
 * Connect request metadata, @code <key> @endcode being the normalized Connect Metadata key
 * (lowercase), the value being the metadata values. Note: Instrumentations SHOULD require an
 * explicit configuration of which metadata values are to be captured. Including all request
 * metadata values can be a security risk - explicit configuration helps avoid leaking sensitive
 * information.
 */
static const char *kRpcConnectRpcRequestMetadata = "rpc.connect_rpc.request.metadata";

/**
 * Connect response metadata, @code <key> @endcode being the normalized Connect Metadata key
 * (lowercase), the value being the metadata values. Note: Instrumentations SHOULD require an
 * explicit configuration of which metadata values are to be captured. Including all response
 * metadata values can be a security risk - explicit configuration helps avoid leaking sensitive
 * information.
 */
static const char *kRpcConnectRpcResponseMetadata = "rpc.connect_rpc.response.metadata";

/**
 * gRPC request metadata, @code <key> @endcode being the normalized gRPC Metadata key (lowercase),
 * the value being the metadata values. Note: Instrumentations SHOULD require an explicit
 * configuration of which metadata values are to be captured. Including all request metadata values
 * can be a security risk - explicit configuration helps avoid leaking sensitive information.
 */
static const char *kRpcGrpcRequestMetadata = "rpc.grpc.request.metadata";

/**
 * gRPC response metadata, @code <key> @endcode being the normalized gRPC Metadata key (lowercase),
 * the value being the metadata values. Note: Instrumentations SHOULD require an explicit
 * configuration of which metadata values are to be captured. Including all response metadata values
 * can be a security risk - explicit configuration helps avoid leaking sensitive information.
 */
static const char *kRpcGrpcResponseMetadata = "rpc.grpc.response.metadata";

/**
 * The <a href="https://github.com/grpc/grpc/blob/v1.33.2/doc/statuscodes.md">numeric status
 * code</a> of the gRPC request.
 */
static const char *kRpcGrpcStatusCode = "rpc.grpc.status_code";

/**
 * @code error.code @endcode property of response if it is an error response.
 */
static const char *kRpcJsonrpcErrorCode = "rpc.jsonrpc.error_code";

/**
 * @code error.message @endcode property of response if it is an error response.
 */
static const char *kRpcJsonrpcErrorMessage = "rpc.jsonrpc.error_message";

/**
 * @code id @endcode property of request or response. Since protocol allows id to be int, string,
 * @code null @endcode or missing (for notifications), value is expected to be cast to string for
 * simplicity. Use empty string in case of @code null @endcode value. Omit entirely if this is a
 * notification.
 */
static const char *kRpcJsonrpcRequestId = "rpc.jsonrpc.request_id";

/**
 * Protocol version as in @code jsonrpc @endcode property of request/response. Since JSON-RPC 1.0
 * doesn't specify this, the value can be omitted.
 */
static const char *kRpcJsonrpcVersion = "rpc.jsonrpc.version";

/**
 * Compressed size of the message in bytes.
 */
static const char *kRpcMessageCompressedSize = "rpc.message.compressed_size";

/**
 * MUST be calculated as two different counters starting from @code 1 @endcode one for sent messages
 * and one for received message. Note: This way we guarantee that the values will be consistent
 * between different implementations.
 */
static const char *kRpcMessageId = "rpc.message.id";

/**
 * Whether this is a received or sent message.
 */
static const char *kRpcMessageType = "rpc.message.type";

/**
 * Uncompressed size of the message in bytes.
 */
static const char *kRpcMessageUncompressedSize = "rpc.message.uncompressed_size";

/**
 * The name of the (logical) method being called, must be equal to the $method part in the span
 * name. Note: This is the logical name of the method from the RPC interface perspective, which can
 * be different from the name of any implementing method/function. The @code code.function @endcode
 * attribute may be used to store the latter (e.g., method actually executing the call on the server
 * side, RPC client stub method on the client side).
 */
static const char *kRpcMethod = "rpc.method";

/**
 * The full (logical) name of the service being called, including its package name, if applicable.
 * Note: This is the logical name of the service from the RPC interface perspective, which can be
 * different from the name of any implementing class. The @code code.namespace @endcode attribute
 * may be used to store the latter (despite the attribute name, it may include a class name; e.g.,
 * class with method actually executing the call on the server side, RPC client stub class on the
 * client side).
 */
static const char *kRpcService = "rpc.service";

/**
 * A string identifying the remoting system. See below for a list of well-known identifiers.
 */
static const char *kRpcSystem = "rpc.system";

// DEBUG: {"brief": "The [error codes](https://connect.build/docs/protocol/#error-codes) of the
// Connect request. Error codes are always string values.", "name": "rpc.connect_rpc.error_code",
// "requirement_level": "recommended", "root_namespace": "rpc", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "cancelled",
// "note": none, "stability": "experimental", "value": "cancelled"}, {"brief": none, "deprecated":
// none, "id": "unknown", "note": none, "stability": "experimental", "value": "unknown"}, {"brief":
// none, "deprecated": none, "id": "invalid_argument", "note": none, "stability": "experimental",
// "value": "invalid_argument"}, {"brief": none, "deprecated": none, "id": "deadline_exceeded",
// "note": none, "stability": "experimental", "value": "deadline_exceeded"}, {"brief": none,
// "deprecated": none, "id": "not_found", "note": none, "stability": "experimental", "value":
// "not_found"}, {"brief": none, "deprecated": none, "id": "already_exists", "note": none,
// "stability": "experimental", "value": "already_exists"}, {"brief": none, "deprecated": none,
// "id": "permission_denied", "note": none, "stability": "experimental", "value":
// "permission_denied"}, {"brief": none, "deprecated": none, "id": "resource_exhausted", "note":
// none, "stability": "experimental", "value": "resource_exhausted"}, {"brief": none, "deprecated":
// none, "id": "failed_precondition", "note": none, "stability": "experimental", "value":
// "failed_precondition"}, {"brief": none, "deprecated": none, "id": "aborted", "note": none,
// "stability": "experimental", "value": "aborted"}, {"brief": none, "deprecated": none, "id":
// "out_of_range", "note": none, "stability": "experimental", "value": "out_of_range"}, {"brief":
// none, "deprecated": none, "id": "unimplemented", "note": none, "stability": "experimental",
// "value": "unimplemented"}, {"brief": none, "deprecated": none, "id": "internal", "note": none,
// "stability": "experimental", "value": "internal"}, {"brief": none, "deprecated": none, "id":
// "unavailable", "note": none, "stability": "experimental", "value": "unavailable"}, {"brief":
// none, "deprecated": none, "id": "data_loss", "note": none, "stability": "experimental", "value":
// "data_loss"}, {"brief": none, "deprecated": none, "id": "unauthenticated", "note": none,
// "stability": "experimental", "value": "unauthenticated"}]}}
namespace RpcConnectRpcErrorCodeValues
{
/**
 * cancelled.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "cancelled", "note": none, "stability":
// "experimental", "value": "cancelled"}
static constexpr const char *kCancelled = "cancelled";
/**
 * unknown.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unknown", "note": none, "stability":
// "experimental", "value": "unknown"}
static constexpr const char *kUnknown = "unknown";
/**
 * invalid_argument.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "invalid_argument", "note": none, "stability":
// "experimental", "value": "invalid_argument"}
static constexpr const char *kInvalidArgument = "invalid_argument";
/**
 * deadline_exceeded.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "deadline_exceeded", "note": none, "stability":
// "experimental", "value": "deadline_exceeded"}
static constexpr const char *kDeadlineExceeded = "deadline_exceeded";
/**
 * not_found.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "not_found", "note": none, "stability":
// "experimental", "value": "not_found"}
static constexpr const char *kNotFound = "not_found";
/**
 * already_exists.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "already_exists", "note": none, "stability":
// "experimental", "value": "already_exists"}
static constexpr const char *kAlreadyExists = "already_exists";
/**
 * permission_denied.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "permission_denied", "note": none, "stability":
// "experimental", "value": "permission_denied"}
static constexpr const char *kPermissionDenied = "permission_denied";
/**
 * resource_exhausted.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "resource_exhausted", "note": none, "stability":
// "experimental", "value": "resource_exhausted"}
static constexpr const char *kResourceExhausted = "resource_exhausted";
/**
 * failed_precondition.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "failed_precondition", "note": none,
// "stability": "experimental", "value": "failed_precondition"}
static constexpr const char *kFailedPrecondition = "failed_precondition";
/**
 * aborted.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "aborted", "note": none, "stability":
// "experimental", "value": "aborted"}
static constexpr const char *kAborted = "aborted";
/**
 * out_of_range.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "out_of_range", "note": none, "stability":
// "experimental", "value": "out_of_range"}
static constexpr const char *kOutOfRange = "out_of_range";
/**
 * unimplemented.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unimplemented", "note": none, "stability":
// "experimental", "value": "unimplemented"}
static constexpr const char *kUnimplemented = "unimplemented";
/**
 * internal.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "internal", "note": none, "stability":
// "experimental", "value": "internal"}
static constexpr const char *kInternal = "internal";
/**
 * unavailable.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unavailable", "note": none, "stability":
// "experimental", "value": "unavailable"}
static constexpr const char *kUnavailable = "unavailable";
/**
 * data_loss.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "data_loss", "note": none, "stability":
// "experimental", "value": "data_loss"}
static constexpr const char *kDataLoss = "data_loss";
/**
 * unauthenticated.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "unauthenticated", "note": none, "stability":
// "experimental", "value": "unauthenticated"}
static constexpr const char *kUnauthenticated = "unauthenticated";
}  // namespace RpcConnectRpcErrorCodeValues

// DEBUG: {"brief": "The [numeric status
// code](https://github.com/grpc/grpc/blob/v1.33.2/doc/statuscodes.md) of the gRPC request.",
// "name": "rpc.grpc.status_code", "requirement_level": "recommended", "root_namespace": "rpc",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "OK",
// "deprecated": none, "id": "ok", "note": none, "stability": "experimental", "value": 0}, {"brief":
// "CANCELLED", "deprecated": none, "id": "cancelled", "note": none, "stability": "experimental",
// "value": 1}, {"brief": "UNKNOWN", "deprecated": none, "id": "unknown", "note": none, "stability":
// "experimental", "value": 2}, {"brief": "INVALID_ARGUMENT", "deprecated": none, "id":
// "invalid_argument", "note": none, "stability": "experimental", "value": 3}, {"brief":
// "DEADLINE_EXCEEDED", "deprecated": none, "id": "deadline_exceeded", "note": none, "stability":
// "experimental", "value": 4}, {"brief": "NOT_FOUND", "deprecated": none, "id": "not_found",
// "note": none, "stability": "experimental", "value": 5}, {"brief": "ALREADY_EXISTS", "deprecated":
// none, "id": "already_exists", "note": none, "stability": "experimental", "value": 6}, {"brief":
// "PERMISSION_DENIED", "deprecated": none, "id": "permission_denied", "note": none, "stability":
// "experimental", "value": 7}, {"brief": "RESOURCE_EXHAUSTED", "deprecated": none, "id":
// "resource_exhausted", "note": none, "stability": "experimental", "value": 8}, {"brief":
// "FAILED_PRECONDITION", "deprecated": none, "id": "failed_precondition", "note": none,
// "stability": "experimental", "value": 9}, {"brief": "ABORTED", "deprecated": none, "id":
// "aborted", "note": none, "stability": "experimental", "value": 10}, {"brief": "OUT_OF_RANGE",
// "deprecated": none, "id": "out_of_range", "note": none, "stability": "experimental", "value":
// 11}, {"brief": "UNIMPLEMENTED", "deprecated": none, "id": "unimplemented", "note": none,
// "stability": "experimental", "value": 12}, {"brief": "INTERNAL", "deprecated": none, "id":
// "internal", "note": none, "stability": "experimental", "value": 13}, {"brief": "UNAVAILABLE",
// "deprecated": none, "id": "unavailable", "note": none, "stability": "experimental", "value": 14},
// {"brief": "DATA_LOSS", "deprecated": none, "id": "data_loss", "note": none, "stability":
// "experimental", "value": 15}, {"brief": "UNAUTHENTICATED", "deprecated": none, "id":
// "unauthenticated", "note": none, "stability": "experimental", "value": 16}]}}
namespace RpcGrpcStatusCodeValues
{
/**
 * OK.
 */
// DEBUG: {"brief": "OK", "deprecated": none, "id": "ok", "note": none, "stability": "experimental",
// "value": 0}
static constexpr int kOk = 0;
/**
 * CANCELLED.
 */
// DEBUG: {"brief": "CANCELLED", "deprecated": none, "id": "cancelled", "note": none, "stability":
// "experimental", "value": 1}
static constexpr int kCancelled = 1;
/**
 * UNKNOWN.
 */
// DEBUG: {"brief": "UNKNOWN", "deprecated": none, "id": "unknown", "note": none, "stability":
// "experimental", "value": 2}
static constexpr int kUnknown = 2;
/**
 * INVALID_ARGUMENT.
 */
// DEBUG: {"brief": "INVALID_ARGUMENT", "deprecated": none, "id": "invalid_argument", "note": none,
// "stability": "experimental", "value": 3}
static constexpr int kInvalidArgument = 3;
/**
 * DEADLINE_EXCEEDED.
 */
// DEBUG: {"brief": "DEADLINE_EXCEEDED", "deprecated": none, "id": "deadline_exceeded", "note":
// none, "stability": "experimental", "value": 4}
static constexpr int kDeadlineExceeded = 4;
/**
 * NOT_FOUND.
 */
// DEBUG: {"brief": "NOT_FOUND", "deprecated": none, "id": "not_found", "note": none, "stability":
// "experimental", "value": 5}
static constexpr int kNotFound = 5;
/**
 * ALREADY_EXISTS.
 */
// DEBUG: {"brief": "ALREADY_EXISTS", "deprecated": none, "id": "already_exists", "note": none,
// "stability": "experimental", "value": 6}
static constexpr int kAlreadyExists = 6;
/**
 * PERMISSION_DENIED.
 */
// DEBUG: {"brief": "PERMISSION_DENIED", "deprecated": none, "id": "permission_denied", "note":
// none, "stability": "experimental", "value": 7}
static constexpr int kPermissionDenied = 7;
/**
 * RESOURCE_EXHAUSTED.
 */
// DEBUG: {"brief": "RESOURCE_EXHAUSTED", "deprecated": none, "id": "resource_exhausted", "note":
// none, "stability": "experimental", "value": 8}
static constexpr int kResourceExhausted = 8;
/**
 * FAILED_PRECONDITION.
 */
// DEBUG: {"brief": "FAILED_PRECONDITION", "deprecated": none, "id": "failed_precondition", "note":
// none, "stability": "experimental", "value": 9}
static constexpr int kFailedPrecondition = 9;
/**
 * ABORTED.
 */
// DEBUG: {"brief": "ABORTED", "deprecated": none, "id": "aborted", "note": none, "stability":
// "experimental", "value": 10}
static constexpr int kAborted = 10;
/**
 * OUT_OF_RANGE.
 */
// DEBUG: {"brief": "OUT_OF_RANGE", "deprecated": none, "id": "out_of_range", "note": none,
// "stability": "experimental", "value": 11}
static constexpr int kOutOfRange = 11;
/**
 * UNIMPLEMENTED.
 */
// DEBUG: {"brief": "UNIMPLEMENTED", "deprecated": none, "id": "unimplemented", "note": none,
// "stability": "experimental", "value": 12}
static constexpr int kUnimplemented = 12;
/**
 * INTERNAL.
 */
// DEBUG: {"brief": "INTERNAL", "deprecated": none, "id": "internal", "note": none, "stability":
// "experimental", "value": 13}
static constexpr int kInternal = 13;
/**
 * UNAVAILABLE.
 */
// DEBUG: {"brief": "UNAVAILABLE", "deprecated": none, "id": "unavailable", "note": none,
// "stability": "experimental", "value": 14}
static constexpr int kUnavailable = 14;
/**
 * DATA_LOSS.
 */
// DEBUG: {"brief": "DATA_LOSS", "deprecated": none, "id": "data_loss", "note": none, "stability":
// "experimental", "value": 15}
static constexpr int kDataLoss = 15;
/**
 * UNAUTHENTICATED.
 */
// DEBUG: {"brief": "UNAUTHENTICATED", "deprecated": none, "id": "unauthenticated", "note": none,
// "stability": "experimental", "value": 16}
static constexpr int kUnauthenticated = 16;
}  // namespace RpcGrpcStatusCodeValues

// DEBUG: {"brief": "Whether this is a received or sent message.", "name": "rpc.message.type",
// "requirement_level": "recommended", "root_namespace": "rpc", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "sent",
// "note": none, "stability": "experimental", "value": "SENT"}, {"brief": none, "deprecated": none,
// "id": "received", "note": none, "stability": "experimental", "value": "RECEIVED"}]}}
namespace RpcMessageTypeValues
{
/**
 * sent.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "sent", "note": none, "stability":
// "experimental", "value": "SENT"}
static constexpr const char *kSent = "SENT";
/**
 * received.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "received", "note": none, "stability":
// "experimental", "value": "RECEIVED"}
static constexpr const char *kReceived = "RECEIVED";
}  // namespace RpcMessageTypeValues

// DEBUG: {"brief": "A string identifying the remoting system. See below for a list of well-known
// identifiers.", "name": "rpc.system", "requirement_level": "recommended", "root_namespace": "rpc",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "gRPC",
// "deprecated": none, "id": "grpc", "note": none, "stability": "experimental", "value": "grpc"},
// {"brief": "Java RMI", "deprecated": none, "id": "java_rmi", "note": none, "stability":
// "experimental", "value": "java_rmi"}, {"brief": ".NET WCF", "deprecated": none, "id":
// "dotnet_wcf", "note": none, "stability": "experimental", "value": "dotnet_wcf"}, {"brief":
// "Apache Dubbo", "deprecated": none, "id": "apache_dubbo", "note": none, "stability":
// "experimental", "value": "apache_dubbo"}, {"brief": "Connect RPC", "deprecated": none, "id":
// "connect_rpc", "note": none, "stability": "experimental", "value": "connect_rpc"}]}}
namespace RpcSystemValues
{
/**
 * gRPC.
 */
// DEBUG: {"brief": "gRPC", "deprecated": none, "id": "grpc", "note": none, "stability":
// "experimental", "value": "grpc"}
static constexpr const char *kGrpc = "grpc";
/**
 * Java RMI.
 */
// DEBUG: {"brief": "Java RMI", "deprecated": none, "id": "java_rmi", "note": none, "stability":
// "experimental", "value": "java_rmi"}
static constexpr const char *kJavaRmi = "java_rmi";
/**
 * .NET WCF.
 */
// DEBUG: {"brief": ".NET WCF", "deprecated": none, "id": "dotnet_wcf", "note": none, "stability":
// "experimental", "value": "dotnet_wcf"}
static constexpr const char *kDotnetWcf = "dotnet_wcf";
/**
 * Apache Dubbo.
 */
// DEBUG: {"brief": "Apache Dubbo", "deprecated": none, "id": "apache_dubbo", "note": none,
// "stability": "experimental", "value": "apache_dubbo"}
static constexpr const char *kApacheDubbo = "apache_dubbo";
/**
 * Connect RPC.
 */
// DEBUG: {"brief": "Connect RPC", "deprecated": none, "id": "connect_rpc", "note": none,
// "stability": "experimental", "value": "connect_rpc"}
static constexpr const char *kConnectRpc = "connect_rpc";
}  // namespace RpcSystemValues

}  // namespace rpc
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
