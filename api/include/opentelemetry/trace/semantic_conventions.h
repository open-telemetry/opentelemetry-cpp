// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

#define OTEL_TRACE_GENERATE_SEMCONV_METHOD(attribute_name, attribute_value) \
  static constexpr const char *GetAttribute##attribute_name() noexcept { return attribute_value; }

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

  // General network connection attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetTransport, "net.transport")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetPeerIp, "net.peer.ip")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetPeerPort, "net.peer.port")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetPeerName, "net.peer.name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetHostIp, "net.host.ip")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetHostPort, "net.host.port")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(NetHostName, "net.host.name")

  // General identity attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(EnduserId, "enduser.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(EnduserRole, "enduser.role")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(EnduserScope, "enduser.scope")

  // General remote service attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(PeerService, "peer.service")

  // General thread attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ThreadId, "thread.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ThreadName, "thread.name")

  // Source Code Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(CodeFunction, "code.function")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(CodeNamespace, "code.namespace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(CodeFilepath, "code.filepath")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(CodeLineno, "code.lineno")

  // Http Span Common Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpMethod, "http.method")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpUrl, "http.url")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpTarget, "http.target")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpHost, "http.host")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpScheme, "http.scheme")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpStatusCode, "http.status_code")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpFlavor, "http.flavor")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpUserAgent, "http.user_agent")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpRequestContentLength, "http.request_content_length")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpRequestContentLengthUncompressed,
                                     "http.request_content_length_uncompressed")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpResponseContentLength, "http.response_content_length")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpResponseContentLengthUncompressed,
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
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpServerName, "http.server_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpRoute, "http.route")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(HttpClientIp, "http.client_ip")

  // DB: Connection-level attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbSystem, "db.system")  // other_sql, mssql, mysql...
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbConnectionString, "db.connection_string")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbUser, "db.user")
  // DB: Connection-level attributes for specific technologies
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbMssqlInstanceName, "db.mssql.instance_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbJdbcDriverClassname, "db.jdbc.driver_classname")
  // DB: Call-level attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbName, "db.name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbStatement, "db.statement")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbOperation, "db.operation")
  // DB: Call-level attributes for specific technologies
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbHbaseNamespace, "db.hbase.namespace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbRedisDatabaseIndex, "db.redis.database_index")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbMongodbCollection, "db.mongodb.collection")

  // // DB: Call-level attributes for Cassandra for clarity
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraKeyspace, "db.cassandra.keyspace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraPageSize, "db.cassandra.page_size")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraConsistencyLevel, "db.cassandra.consistency_level")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraTable, "db.cassandra.table")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraIdempotence, "db.cassandra.idempotence")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraSpeculativeExecutionCount,
                                     "db.cassandra.speculative_execution_count")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraCoordinatorId, "db.cassandra.coordinator.id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(DbCassandraCoordinatorDC, "db.cassandra.coordinator.dc")

  // Common RPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcSystem, "rpc.system")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcService, "rpc.service")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcMethod, "rpc.method")
  // gRPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcGrpcStatusCode, "rpc.grpc.status_code")
  // JSON-RPC attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcJsonrpcVersion, "rpc.jsonrpc.version")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcJsonrpcRequestId, "rpc.jsonrpc.request_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcJsonrpcErrorCode, "rpc.jsonrpc.error_code")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(RpcJsonrpcErrorMessage, "rpc.jsonrpc.error_message")

  // faas: General Attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasTrigger, "faas.trigger")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasExecution, "faas.execution")
  // faas: incoming invocations
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasColdStart, "faas.coldstart")
  // faas: outgoing invocations
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasInvokedName, "faas.invoked_name")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasInvokedProvider, "faas.invoked_provider")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasInvokedRegion, "faas.invoked_region")
  // faas: datastore trigger
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasDocumentCollection, "faas.document.collection")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasDocumentOperation, "faas.document.operation")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasDocumentTime, "faas.document.time")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasDocumentName, "faas.document.name")
  // faas: timer trigger
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasTime, "faas.time")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(FaasCron, "faas.cron")

  // messaging attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingSystem, "messaging.system")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingDestination, "messaging.destination")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingDestinationKind, "messaging.destination_kind")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingTempDestination, "messaging.temp_destination")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingProtocol, "messaging.protocol")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingProtocolVersion, "messaging.protocol_version")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingUrl, "messaging.url")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingMessageId, "messaging.message_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingConversationId, "messaging.conversation_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingPayloadSize, "messaging.message_payload_size_bytes")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingPayloadCompressedSize,
                                     "messaging.message_payload_compressed_size_bytes")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingOperation, "messaging.operation")
  // messaging attributes specific to messaging systems
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingRabbitMQRoutingKey, "messaging.rabbitmq.routing_key")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingKafkaMessageKey, "messaging.kafka.message_key")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingKafkaConsumerGroup, "messaging.kafka.consumer_group")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingKafkaClientId, "messaging.kafka.client_id")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingKafkaPartition, "messaging.kafka.partition")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(MessagingKafkaTombstone, "messaging.kafka.tombstone")

  // Exceptions attributes
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ExceptionType, "exception.type")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ExceptionMessage, "exception.message")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ExceptionStacktrace, "exception.stacktrace")
  OTEL_TRACE_GENERATE_SEMCONV_METHOD(ExceptionEscapted, "exception.escaped")
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
