// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// NOTE:
// This implementation is based on the experimental specs for trace semantic convention as defined
// here:
// https://github.com/open-telemetry/opentelemetry-specification/tree/v1.0.0/specification/trace/semantic_conventions
// and MAY will change in future.

#pragma once

#include "opentelemetry/common/string_util.h"
#include "opentelemetry/version.h"

#define OTEL_GET_TRACE_ATTR(name) opentelemetry::trace::attr(OTEL_CPP_CONST_HASHCODE(name))

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

/**
 * Stores the Constants for semantic kAttribute names outlined by the OpenTelemetry specifications.
 * <see
 * href="https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/semantic_conventions/README.md"/>.
 */
static const struct
{
  uint32_t attribute_id;
  const char *attribute_key;
} attribute_ids[] = {
    // The set of constants matches the specification as of this commit.
    // https://github.com/open-telemetry/opentelemetry-specification/tree/main/specification/trace/semantic_conventions
    // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/trace/semantic_conventions/exceptions.md

    // General network connection attributes
    {OTEL_CPP_CONST_HASHCODE(AttrNetTransport), "net.transport"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetPeerIp), "net.peer.ip"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetPeerPort), "net.peer.port"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetPeerName), "net.peer.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetHostIp), "net.host.ip"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetHostPort), "net.host.port"},
    {OTEL_CPP_CONST_HASHCODE(AttrNetHostName), "net.host.name"},

    // General identity attributes
    {OTEL_CPP_CONST_HASHCODE(AttrEnduserId), "enduser.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrEnduserRole), "enduser.role"},
    {OTEL_CPP_CONST_HASHCODE(AttrEnduserScope), "enduser.scope"},

    // General remote service attributes
    {OTEL_CPP_CONST_HASHCODE(AttrPeerService), "peer.service"},

    // General thread attributes
    {OTEL_CPP_CONST_HASHCODE(AttrThreadId), "thread.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrThreadName), "thread.name"},

    // Source Code Attributes
    {OTEL_CPP_CONST_HASHCODE(AttrCodeFunction), "code.function"},
    {OTEL_CPP_CONST_HASHCODE(AttrCodeNamespace), "code.namespace"},
    {OTEL_CPP_CONST_HASHCODE(AttrCodeFilepath), "code.filepath"},
    {OTEL_CPP_CONST_HASHCODE(AttrCodeLineno), "code.lineno"},

    // Http Span Common Attributes
    {OTEL_CPP_CONST_HASHCODE(AttrHttpMethod), "http.method"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpUrl), "http.url"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpTarget), "http.target"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpHost), "http.host"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpScheme), "http.scheme"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpStatusCode), "http.status_code"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpFlavor), "http.flavor"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpUserAgent), "http.user_agent"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpRequestContentLength), "http.request_content_length"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpRequestContentLengthUncompressed),
     "http.request_content_length_uncompressed"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpResponseContentLength), "http.response_content_length"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpResponseContentLengthUncompressed),
     "http.response_content_length_uncompressed"},

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
    {OTEL_CPP_CONST_HASHCODE(AttrHttpServerName), "http.server_name"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpRoute), "http.route"},
    {OTEL_CPP_CONST_HASHCODE(AttrHttpClientIp), "http.client_ip"},

    // DB: Connection-level attributes
    {OTEL_CPP_CONST_HASHCODE(AttrDbSystem), "db.system"},  // other_sql, mssql, mysql...
    {OTEL_CPP_CONST_HASHCODE(AttrDbConnectionString), "db.connection_string"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbUser), "db.user"},
    // DB: Connection-level attributes for specific technologies
    {OTEL_CPP_CONST_HASHCODE(AttrDbMssqlInstanceName), "db.mssql.instance_name"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbJdbcDriverClassname), "db.jdbc.driver_classname"},
    // DB: Call-level attributes
    {OTEL_CPP_CONST_HASHCODE(AttrDbName), "db.name"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbStatement), "db.statement"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbOperation), "db.operation"},
    // DB: Call-level attributes for specific technologies
    {OTEL_CPP_CONST_HASHCODE(AttrDbHbaseNamespace), "db.hbase.namespace"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbRedisDatabaseIndex), "db.redis.database_index"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbMongodbCollection), "db.mongodb.collection"},

    // // DB: Call-level attributes for Cassandra for clarity
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraKeyspace), "db.cassandra.keyspace"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraPageSize), "db.cassandra.page_size"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraConsistencyLevel), "db.cassandra.consistency_level"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraTable), "db.cassandra.table"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraIdempotence), "db.cassandra.idempotence"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraSpeculativeExecutionCount),
     "db.cassandra.speculative_execution_count"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraCoordinatorId), "db.cassandra.coordinator.id"},
    {OTEL_CPP_CONST_HASHCODE(AttrDbCassandraCoordinatorDC), "db.cassandra.coordinator.dc"},

    // Common RPC attributes
    {OTEL_CPP_CONST_HASHCODE(AttrRpcSystem), "rpc.system"},
    {OTEL_CPP_CONST_HASHCODE(AttrRpcService), "rpc.service"},
    {OTEL_CPP_CONST_HASHCODE(AttrRpcMethod), "rpc.method"},
    // gRPC attributes
    {OTEL_CPP_CONST_HASHCODE(AttrRpcGrpcStatusCode), "rpc.grpc.status_code"},
    // JSON-RPC attributes
    {OTEL_CPP_CONST_HASHCODE(AttrRpcJsonrpcVersion), "rpc.jsonrpc.version"},
    {OTEL_CPP_CONST_HASHCODE(AttrRpcJsonrpcRequestId), "rpc.jsonrpc.request_id"},
    {OTEL_CPP_CONST_HASHCODE(AttrRpcJsonrpcErrorCode), "rpc.jsonrpc.error_code"},
    {OTEL_CPP_CONST_HASHCODE(AttrRpcJsonrpcErrorMessage), "rpc.jsonrpc.error_message"},

    // faas: General Attributes
    {OTEL_CPP_CONST_HASHCODE(AttrFaasTrigger), "faas.trigger"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasExecution), "faas.execution"},
    // faas: incoming invocations
    {OTEL_CPP_CONST_HASHCODE(AttrFaasColdStart), "faas.coldstart"},
    // faas: outgoing invocations
    {OTEL_CPP_CONST_HASHCODE(AttrFaasInvokedName), "faas.invoked_name"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasInvokedProvider), "faas.invoked_provider"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasInvokedRegion), "faas.invoked_region"},
    // faas: datastore trigger
    {OTEL_CPP_CONST_HASHCODE(AttrFaasDocumentCollection), "faas.document.collection"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasDocumentOperation), "faas.document.operation"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasDocumentTime), "faas.document.time"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasDocumentName), "faas.document.name"},
    // faas: timer trigger
    {OTEL_CPP_CONST_HASHCODE(AttrFaasTime), "faas.time"},
    {OTEL_CPP_CONST_HASHCODE(AttrFaasCron), "faas.cron"},

    // messaging attributes
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingSystem), "messaging.system"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingDestination), "messaging.destination"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingDestinationKind), "messaging.destination_kind"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingTempDestination), "messaging.temp_destination"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingProtocol), "messaging.protocol"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingProtocolVersion), "messaging.protocol_version"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingUrl), "messaging.url"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingMessageId), "messaging.message_id"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingConversationId), "messaging.conversation_id"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingPayloadSize), "messaging.message_payload_size_bytes"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingPayloadCompressedSize),
     "messaging.message_payload_compressed_size_bytes"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingOperation), "messaging.operation"},
    // messaging attributes specific to messaging systems
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingRabbitMQRoutingKey), "messaging.rabbitmq.routing_key"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingKafkaMessageKey), "messaging.kafka.message_key"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingKafkaConsumerGroup), "messaging.kafka.consumer_group"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingKafkaClientId), "messaging.kafka.client_id"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingKafkaPartition), "messaging.kafka.partition"},
    {OTEL_CPP_CONST_HASHCODE(AttrMessagingKafkaTombstone), "messaging.kafka.tombstone"},

    // Exceptions attributes
    {OTEL_CPP_CONST_HASHCODE(AttrExceptionType), "exception.type"},
    {OTEL_CPP_CONST_HASHCODE(AttrExceptionMessage), "exception.message"},
    {OTEL_CPP_CONST_HASHCODE(AttrExceptionStacktrace), "exception.stacktrace"},
    {OTEL_CPP_CONST_HASHCODE(AttrExceptionEscapted), "exception.escaped"},
};
// function to generate hash code for semantic conventions attributes.

#define OTEL_CPP_TRACE_ATTRIBUTES_MAX (sizeof(attribute_ids) / sizeof(attribute_ids[0]))

inline const char *attr(uint32_t attr)
{
  for (size_t i = 0; i < OTEL_CPP_TRACE_ATTRIBUTES_MAX; i++)
  {
    if (attribute_ids[i].attribute_id == attr)
      return attribute_ids[i].attribute_key;
  }
  return "";
}

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
