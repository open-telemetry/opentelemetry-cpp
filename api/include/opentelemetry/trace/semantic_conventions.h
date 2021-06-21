// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

#define OTEL_TRACE_GENERATE_SEMCONV_METHOD(attribute_name, attribute_value) \
  static const nostd::string_view Get##attribute_name() noexcept            \
  {                                                                         \
    static const nostd::string_view attribute_name = attribute_value;       \
    return attribute_name;                                                  \
  }

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

/**
 * Stores the Constants for semantic kAttribute names outlined by the OpenTelemetry specifications.
 * <see
 * href="https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/semantic_conventions/README.md"/>.
 */
class SemanticConventions final
{
public:
  // The set of constants matches the specification as of this commit.
  // https://github.com/open-telemetry/opentelemetry-specification/tree/main/specification/trace/semantic_conventions
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/semantic_conventions/exceptions.md

  // Genetal network connection attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetTransport, "net.transport")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetPeerIp, "net.peer.ip")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetPeerPort, "net.peer.port")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetPeerName, "net.peer.name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetHostIp, "net.host.ip")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetHostPort, "net.host.port")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeNetHostName, "net.host.name")

  // General identity attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeEnduserId, "enduser.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeEnduserRole, "enduser.role")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeEnduserScope, "enduser.scope")

  // General remote service attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributePeerService, "peer.service")

  // General thread attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeThreadId, "thread.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeThreadName, "thread.name")

  // Source Code Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeCodeFunction, "code.function")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeCodeNamespace, "code.namespace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeCodeFilepath, "code.filepath")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeCodeLineno, "code.lineno")

  // Http Span Common Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpMethod, "http.method")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpUrl, "http.url")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpTarget, "http.target")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpHost, "http.host")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpScheme, "http.scheme")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpStatusCode, "http.status_code")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpFlavor, "http.flavor")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpUserAgent, "http.user_agent")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpRequestContentLength,
                                     "http.request_content_length")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpRequestContentLengthUncompressed,
                                     "http.request_content_length_uncompressed")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpResponseContentLength,
                                     "http.response_content_length")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpResponseContentLengthUncompressed,
                                     "http.response_content_length_uncompressed")

  // HTTP Span Client Attributes
  // One of the following combinations:
  //   - http.url
  //   - http.scheme, http.host, http.target
  //   - http.scheme, net.peer.name, net.peer.port, http.target
  //   - http.scheme, net.peer.ip, net.peer.port, http.target

  // HTTP Span Server Attributes
  // One of the following combinations:
  //  -http.scheme, http.host, http.target
  //  -http.scheme, http.server_name, net.host.port, http.target
  //  -http.scheme, net.host.name, net.host.port, http.target
  //  -http.url
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpServerName, "http.server_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpRoute, "http.route")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeHttpClientIp, "http.client_ip")

  // DB: Connection-level attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbSystem, "db.system")  // other_sql, mssql, mysql...
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbConnectionString, "db.connection_string")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbUser, "db.user")
  // DB: Connection-level attributes for specific technologies
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbMssqlInstanceName, "db.mssql.instance_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbJdbcDriverClassname, "db.jdbc.driver_classname")
  // DB: Call-level attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbName, "db.name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbStatement, "db.statement")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbOperation, "db.operation")
  // DB: Call-level attributes for specific technologies
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbHbaseNamespace, "db.hbase.namespace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbRedisDatabaseIndex, "db.redis.database_index")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbMongodbCollection, "db.mongodb.collection")

  // // DB: Call-level attributes for Cassandra for clarity
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraKeyspace, "db.cassandra.keyspace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraPageSize, "db.cassandra.page_size")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraConsistencyLevel,
                                     "db.cassandra.consistency_level")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraTable, "db.cassandra.table")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraIdempotence, "db.cassandra.idempotence")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraSpeculativeExecutionCount,
                                     "db.cassandra.speculative_execution_count")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraCoordinatorId,
                                     "db.cassandra.coordinator.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeDbCassandraCoordinatorDC,
                                     "db.cassandra.coordinator.dc")

  // Common RPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcSystem, "rpc.system")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcService, "rpc.service")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcMethod, "rpc.method")
  // gRPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcGrpcStatusCode, "rpc.grpc.status_code")
  // JSON-RPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcJsonrpcVersion, "rpc.jsonrpc.version")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcJsonrpcRequestId, "rpc.jsonrpc.request_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcJsonrpcErrorCode, "rpc.jsonrpc.error_code")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeRpcJsonrpcErrorMessage, "rpc.jsonrpc.error_message")

  // faas: General Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasTrigger, "faas.trigger")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasExecution, "faas.execution")
  // faas: incoming invocations
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasColdStart, "faas.coldstart")
  // faas: outgoing invocations
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasInvokedName, "faas.invoked_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasInvokedProvider, "faas.invoked_provider")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasInvokedRegion, "faas.invoked_region")
  // faas: datastore trigger
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasDocumentCollection, "faas.document.collection")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasDocumentOperation, "faas.document.operation")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasDocumentTime, "faas.document.time")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasDocumentName, "faas.document.name")
  // faas: timer trigger
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasTime, "faas.time")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeFaasCron, "faas.cron")

  // messaging attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingSystem, "messaging.system")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingDestination, "messaging.destination")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingDestinationKind,
                                     "messaging.destination_kind")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingTempDestination,
                                     "messaging.temp_destination")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingProtocol, "messaging.protocol")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingProtocolVersion,
                                     "messaging.protocol_version")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingUrl, "messaging.url")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingMessageId, "messaging.message_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingConversationId, "messaging.conversation_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingPayloadSize,
                                     "messaging.message_payload_size_bytes")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingPayloadCompressedSize,
                                     "messaging.message_payload_compressed_size_bytes")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingOperation, "messaging.operation")
  // messaging attributes specific to messaging systems
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingRabbitMQRoutingKey,
                                     "messaging.rabbitmq.routing_key")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingKafkaMessageKey,
                                     "messaging.kafka.message_key")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingKafkaConsumerGroup,
                                     "messaging.kafka.consumer_group")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingKafkaClientId, "messaging.kafka.client_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingKafkaPartition, "messaging.kafka.partition")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeMessagingKafkaTombstone, "messaging.kafka.tombstone")

  // Exceptions attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeExceptionType, "exception.type")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeExceptionMessage, "exception.message")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeExceptionStacktrace, "exception.stacktrace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(AttributeExceptionEscapted, "exception.escaped")
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE