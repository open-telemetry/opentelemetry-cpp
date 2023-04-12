/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
  DO NOT EDIT, this is an Auto-generated file
  from buildscripts/semantic-convention/templates/SemanticAttributes.h.j2
*/

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

namespace SemanticConventions
{
/**
 * The URL of the OpenTelemetry schema for these keys and values.
 */
static constexpr const char *kSchemaUrl = "https://opentelemetry.io/schemas/1.20.0";

/**
 * The type of the exception (its fully-qualified class name, if applicable). The dynamic type of
 * the exception should be preferred over the static type in languages that support it.
 */
static constexpr const char *kExceptionType = "exception.type";

/**
 * The exception message.
 */
static constexpr const char *kExceptionMessage = "exception.message";

/**
 * A stacktrace as a string in the natural representation for the language runtime. The
 * representation is to be determined and documented by each language SIG.
 */
static constexpr const char *kExceptionStacktrace = "exception.stacktrace";

/**
 * HTTP request method.
 */
static constexpr const char *kHttpMethod = "http.method";

/**
 * <a href="https://tools.ietf.org/html/rfc7231#section-6">HTTP response status code</a>.
 */
static constexpr const char *kHttpStatusCode = "http.status_code";

/**
 * The URI scheme identifying the used protocol.
 */
static constexpr const char *kHttpScheme = "http.scheme";

/**
 * The matched route (path template in the format used by the respective server framework). See note
below
 *
 * <p>Notes:
  <ul> <li>MUST NOT be populated when this is not supported by the HTTP server framework as the
route attribute should have low-cardinality and the URI path can NOT substitute it. SHOULD include
the <a href="/specification/trace/semantic_conventions/http.md#http-server-definitions">application
root</a> if there is one.</li> </ul>
 */
static constexpr const char *kHttpRoute = "http.route";

/**
 * The name identifies the event.
 */
static constexpr const char *kEventName = "event.name";

/**
 * The domain identifies the business context for the events.
 *
 * <p>Notes:
  <ul> <li>Events across different domains may have same {@code event.name}, yet be
unrelated events.</li> </ul>
 */
static constexpr const char *kEventDomain = "event.domain";

/**
 * A unique identifier for the Log Record.
 *
 * <p>Notes:
  <ul> <li>If an id is provided, other log records with the same id will be considered duplicates
and can be removed safely. This means, that two distinguishable log records MUST have different
values. The id MAY be an <a href="https://github.com/ulid/spec">Universally Unique Lexicographically
Sortable Identifier (ULID)</a>, but other identifiers (e.g. UUID) may be used as needed.</li> </ul>
 */
static constexpr const char *kLogRecordUid = "log.record.uid";

/**
 * The full invoked ARN as provided on the {@code Context} passed to the function ({@code
 Lambda-Runtime-Invoked-Function-Arn} header on the {@code /runtime/invocation/next} applicable).
 *
 * <p>Notes:
  <ul> <li>This may be different from {@code cloud.resource_id} if an alias is involved.</li> </ul>
 */
static constexpr const char *kAwsLambdaInvokedArn = "aws.lambda.invoked_arn";

/**
 * The <a href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#id">event_id</a>
 * uniquely identifies the event.
 */
static constexpr const char *kCloudeventsEventId = "cloudevents.event_id";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#source-1">source</a>
 * identifies the context in which an event happened.
 */
static constexpr const char *kCloudeventsEventSource = "cloudevents.event_source";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#specversion">version of
 * the CloudEvents specification</a> which the event uses.
 */
static constexpr const char *kCloudeventsEventSpecVersion = "cloudevents.event_spec_version";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#type">event_type</a>
 * contains a value describing the type of event related to the originating occurrence.
 */
static constexpr const char *kCloudeventsEventType = "cloudevents.event_type";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#subject">subject</a> of
 * the event in the context of the event producer (identified by source).
 */
static constexpr const char *kCloudeventsEventSubject = "cloudevents.event_subject";

/**
 * Parent-child Reference type
 *
 * <p>Notes:
  <ul> <li>The causal relationship between a child Span and a parent Span.</li> </ul>
 */
static constexpr const char *kOpentracingRefType = "opentracing.ref_type";

/**
 * An identifier for the database management system (DBMS) product being used. See below for a list
 * of well-known identifiers.
 */
static constexpr const char *kDbSystem = "db.system";

/**
 * The connection string used to connect to the database. It is recommended to remove embedded
 * credentials.
 */
static constexpr const char *kDbConnectionString = "db.connection_string";

/**
 * Username for accessing the database.
 */
static constexpr const char *kDbUser = "db.user";

/**
 * The fully-qualified class name of the <a
 * href="https://docs.oracle.com/javase/8/docs/technotes/guides/jdbc/">Java Database Connectivity
 * (JDBC)</a> driver used to connect.
 */
static constexpr const char *kDbJdbcDriverClassname = "db.jdbc.driver_classname";

/**
 * This attribute is used to report the name of the database being accessed. For commands that
 switch the database, this should be set to the target database (even if the command fails).
 *
 * <p>Notes:
  <ul> <li>In some SQL databases, the database name to be used is called &quot;schema name&quot;. In
 case there are multiple layers that could be considered for database name (e.g. Oracle instance
 name and schema name), the database name to be used is the more specific layer (e.g. Oracle schema
 name).</li> </ul>
 */
static constexpr const char *kDbName = "db.name";

/**
 * The database statement being executed.
 */
static constexpr const char *kDbStatement = "db.statement";

/**
 * The name of the operation being executed, e.g. the <a
 href="https://docs.mongodb.com/manual/reference/command/#database-operations">MongoDB command
 name</a> such as {@code findAndModify}, or the SQL keyword.
 *
 * <p>Notes:
  <ul> <li>When setting this to an SQL keyword, it is not recommended to attempt any client-side
 parsing of {@code db.statement} just to get this property, but it should be set if the operation
 name is provided by the library being instrumented. If the SQL statement has an ambiguous
 operation, or performs more than one operation, this value may be omitted.</li> </ul>
 */
static constexpr const char *kDbOperation = "db.operation";

/**
 * The Microsoft SQL Server <a
 href="https://docs.microsoft.com/en-us/sql/connect/jdbc/building-the-connection-url?view=sql-server-ver15">instance
 name</a> connecting to. This name is used to determine the port of a named instance.
 *
 * <p>Notes:
  <ul> <li>If setting a {@code db.mssql.instance_name}, {@code net.peer.port} is no longer required
 (but still recommended if non-standard).</li> </ul>
 */
static constexpr const char *kDbMssqlInstanceName = "db.mssql.instance_name";

/**
 * The fetch size used for paging, i.e. how many rows will be returned at once.
 */
static constexpr const char *kDbCassandraPageSize = "db.cassandra.page_size";

/**
 * The consistency level of the query. Based on consistency values from <a
 * href="https://docs.datastax.com/en/cassandra-oss/3.0/cassandra/dml/dmlConfigConsistency.html">CQL</a>.
 */
static constexpr const char *kDbCassandraConsistencyLevel = "db.cassandra.consistency_level";

/**
 * The name of the primary table that the operation is acting upon, including the keyspace name (if
 applicable).
 *
 * <p>Notes:
  <ul> <li>This mirrors the db.sql.table attribute but references cassandra rather than sql. It is
 not recommended to attempt any client-side parsing of {@code db.statement} just to get this
 property, but it should be set if it is provided by the library being instrumented. If the
 operation is acting upon an anonymous table, or more than one table, this value MUST NOT be
 set.</li> </ul>
 */
static constexpr const char *kDbCassandraTable = "db.cassandra.table";

/**
 * Whether or not the query is idempotent.
 */
static constexpr const char *kDbCassandraIdempotence = "db.cassandra.idempotence";

/**
 * The number of times a query was speculatively executed. Not set or {@code 0} if the query was not
 * executed speculatively.
 */
static constexpr const char *kDbCassandraSpeculativeExecutionCount =
    "db.cassandra.speculative_execution_count";

/**
 * The ID of the coordinating node for a query.
 */
static constexpr const char *kDbCassandraCoordinatorId = "db.cassandra.coordinator.id";

/**
 * The data center of the coordinating node for a query.
 */
static constexpr const char *kDbCassandraCoordinatorDc = "db.cassandra.coordinator.dc";

/**
 * The index of the database being accessed as used in the <a
 * href="https://redis.io/commands/select">{@code SELECT} command</a>, provided as an integer. To be
 * used instead of the generic {@code db.name} attribute.
 */
static constexpr const char *kDbRedisDatabaseIndex = "db.redis.database_index";

/**
 * The collection being accessed within the database stated in {@code db.name}.
 */
static constexpr const char *kDbMongodbCollection = "db.mongodb.collection";

/**
 * The name of the primary table that the operation is acting upon, including the database name (if
 applicable).
 *
 * <p>Notes:
  <ul> <li>It is not recommended to attempt any client-side parsing of {@code db.statement} just to
 get this property, but it should be set if it is provided by the library being instrumented. If the
 operation is acting upon an anonymous table, or more than one table, this value MUST NOT be
 set.</li> </ul>
 */
static constexpr const char *kDbSqlTable = "db.sql.table";

/**
 * Unique Cosmos client instance id.
 */
static constexpr const char *kDbCosmosdbClientId = "db.cosmosdb.client_id";

/**
 * CosmosDB Operation Type.
 */
static constexpr const char *kDbCosmosdbOperationType = "db.cosmosdb.operation_type";

/**
 * Cosmos client connection mode.
 */
static constexpr const char *kDbCosmosdbConnectionMode = "db.cosmosdb.connection_mode";

/**
 * Cosmos DB container name.
 */
static constexpr const char *kDbCosmosdbContainer = "db.cosmosdb.container";

/**
 * Request payload size in bytes
 */
static constexpr const char *kDbCosmosdbRequestContentLength = "db.cosmosdb.request_content_length";

/**
 * Cosmos DB status code.
 */
static constexpr const char *kDbCosmosdbStatusCode = "db.cosmosdb.status_code";

/**
 * Cosmos DB sub status code.
 */
static constexpr const char *kDbCosmosdbSubStatusCode = "db.cosmosdb.sub_status_code";

/**
 * RU consumed for that operation
 */
static constexpr const char *kDbCosmosdbRequestCharge = "db.cosmosdb.request_charge";

/**
 * Name of the code, either &quot;OK&quot; or &quot;ERROR&quot;. MUST NOT be set if the status code
 * is UNSET.
 */
static constexpr const char *kOtelStatusCode = "otel.status_code";

/**
 * Description of the Status if it has a value, otherwise not set.
 */
static constexpr const char *kOtelStatusDescription = "otel.status_description";

/**
 * Type of the trigger which caused this function invocation.
 *
 * <p>Notes:
  <ul> <li>For the server/consumer span on the incoming side,
{@code faas.trigger} MUST be set.</li><li>Clients invoking FaaS instances usually cannot set {@code
faas.trigger}, since they would typically need to look in the payload to determine the event type.
If clients set it, it should be the same as the trigger that corresponding incoming would have
(i.e., this has nothing to do with the underlying transport used to make the API call to invoke the
lambda, which is often HTTP).</li> </ul>
 */
static constexpr const char *kFaasTrigger = "faas.trigger";

/**
 * The invocation ID of the current function invocation.
 */
static constexpr const char *kFaasInvocationId = "faas.invocation_id";

/**
 * The name of the source on which the triggering operation was performed. For example, in Cloud
 * Storage or S3 corresponds to the bucket name, and in Cosmos DB to the database name.
 */
static constexpr const char *kFaasDocumentCollection = "faas.document.collection";

/**
 * Describes the type of the operation that was performed on the data.
 */
static constexpr const char *kFaasDocumentOperation = "faas.document.operation";

/**
 * A string containing the time when the data was accessed in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static constexpr const char *kFaasDocumentTime = "faas.document.time";

/**
 * The document name/table subjected to the operation. For example, in Cloud Storage or S3 is the
 * name of the file, and in Cosmos DB the table name.
 */
static constexpr const char *kFaasDocumentName = "faas.document.name";

/**
 * A string containing the function invocation time in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static constexpr const char *kFaasTime = "faas.time";

/**
 * A string containing the schedule period as <a
 * href="https://docs.oracle.com/cd/E12058_01/doc/doc.1014/e12030/cron_expressions.htm">Cron
 * Expression</a>.
 */
static constexpr const char *kFaasCron = "faas.cron";

/**
 * A boolean that is true if the serverless function is executed for the first time (aka
 * cold-start).
 */
static constexpr const char *kFaasColdstart = "faas.coldstart";

/**
 * The name of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code faas.name} resource attribute of the invoked function.</li>
 </ul>
 */
static constexpr const char *kFaasInvokedName = "faas.invoked_name";

/**
 * The cloud provider of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code cloud.provider} resource attribute of the invoked
 function.</li> </ul>
 */
static constexpr const char *kFaasInvokedProvider = "faas.invoked_provider";

/**
 * The cloud region of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code cloud.region} resource attribute of the invoked
 function.</li> </ul>
 */
static constexpr const char *kFaasInvokedRegion = "faas.invoked_region";

/**
 * The unique identifier of the feature flag.
 */
static constexpr const char *kFeatureFlagKey = "feature_flag.key";

/**
 * The name of the service provider that performs the flag evaluation.
 */
static constexpr const char *kFeatureFlagProviderName = "feature_flag.provider_name";

/**
 * SHOULD be a semantic identifier for a value. If one is unavailable, a stringified version of the
value can be used.
 *
 * <p>Notes:
  <ul> <li>A semantic identifier, commonly referred to as a variant, provides a means
for referring to a value without including the value itself. This can
provide additional context for understanding the meaning behind a value.
For example, the variant {@code red} maybe be used for the value {@code #c05543}.</li><li>A
stringified version of the value can be used in situations where a semantic identifier is
unavailable. String representation of the value should be determined by the implementer.</li> </ul>
 */
static constexpr const char *kFeatureFlagVariant = "feature_flag.variant";

/**
 * Transport protocol used. See note below.
 */
static constexpr const char *kNetTransport = "net.transport";

/**
 * Application layer protocol used. The value SHOULD be normalized to lowercase.
 */
static constexpr const char *kNetProtocolName = "net.protocol.name";

/**
 * Version of the application layer protocol used. See note below.
 *
 * <p>Notes:
  <ul> <li>{@code net.protocol.version} refers to the version of the protocol used and might be
 different from the protocol client's version. If the HTTP client used has a version of {@code
 0.27.2}, but sends HTTP version {@code 1.1}, this attribute should be set to {@code 1.1}.</li>
 </ul>
 */
static constexpr const char *kNetProtocolVersion = "net.protocol.version";

/**
 * Remote socket peer name.
 */
static constexpr const char *kNetSockPeerName = "net.sock.peer.name";

/**
 * Remote socket peer address: IPv4 or IPv6 for internet protocols, path for local communication, <a
 * href="https://man7.org/linux/man-pages/man7/address_families.7.html">etc</a>.
 */
static constexpr const char *kNetSockPeerAddr = "net.sock.peer.addr";

/**
 * Remote socket peer port.
 */
static constexpr const char *kNetSockPeerPort = "net.sock.peer.port";

/**
 * Protocol <a href="https://man7.org/linux/man-pages/man7/address_families.7.html">address
 * family</a> which is used for communication.
 */
static constexpr const char *kNetSockFamily = "net.sock.family";

/**
 * Logical remote hostname, see note below.
 *
 * <p>Notes:
  <ul> <li>{@code net.peer.name} SHOULD NOT be set if capturing it would require an extra DNS
 lookup.</li> </ul>
 */
static constexpr const char *kNetPeerName = "net.peer.name";

/**
 * Logical remote port number
 */
static constexpr const char *kNetPeerPort = "net.peer.port";

/**
 * Logical local hostname or similar, see note below.
 */
static constexpr const char *kNetHostName = "net.host.name";

/**
 * Logical local port number, preferably the one that the peer used to connect
 */
static constexpr const char *kNetHostPort = "net.host.port";

/**
 * Local socket address. Useful in case of a multi-IP host.
 */
static constexpr const char *kNetSockHostAddr = "net.sock.host.addr";

/**
 * Local socket port number.
 */
static constexpr const char *kNetSockHostPort = "net.sock.host.port";

/**
 * The internet connection type currently being used by the host.
 */
static constexpr const char *kNetHostConnectionType = "net.host.connection.type";

/**
 * This describes more details regarding the connection.type. It may be the type of cell technology
 * connection, but it could be used for describing details about a wifi connection.
 */
static constexpr const char *kNetHostConnectionSubtype = "net.host.connection.subtype";

/**
 * The name of the mobile carrier.
 */
static constexpr const char *kNetHostCarrierName = "net.host.carrier.name";

/**
 * The mobile carrier country code.
 */
static constexpr const char *kNetHostCarrierMcc = "net.host.carrier.mcc";

/**
 * The mobile carrier network code.
 */
static constexpr const char *kNetHostCarrierMnc = "net.host.carrier.mnc";

/**
 * The ISO 3166-1 alpha-2 2-character country code associated with the mobile carrier network.
 */
static constexpr const char *kNetHostCarrierIcc = "net.host.carrier.icc";

/**
 * The <a href="../../resource/semantic_conventions/README.md#service">{@code service.name}</a> of
 * the remote service. SHOULD be equal to the actual {@code service.name} resource attribute of the
 * remote service if any.
 */
static constexpr const char *kPeerService = "peer.service";

/**
 * Username or client_id extracted from the access token or <a
 * href="https://tools.ietf.org/html/rfc7235#section-4.2">Authorization</a> header in the inbound
 * request from outside the system.
 */
static constexpr const char *kEnduserId = "enduser.id";

/**
 * Actual/assumed role the client is making the request under extracted from token or application
 * security context.
 */
static constexpr const char *kEnduserRole = "enduser.role";

/**
 * Scopes or granted authorities the client currently possesses extracted from token or application
 * security context. The value would come from the scope associated with an <a
 * href="https://tools.ietf.org/html/rfc6749#section-3.3">OAuth 2.0 Access Token</a> or an attribute
 * value in a <a
 * href="http://docs.oasis-open.org/security/saml/Post2.0/sstc-saml-tech-overview-2.0.html">SAML 2.0
 * Assertion</a>.
 */
static constexpr const char *kEnduserScope = "enduser.scope";

/**
 * Current &quot;managed&quot; thread ID (as opposed to OS thread ID).
 */
static constexpr const char *kThreadId = "thread.id";

/**
 * Current thread name.
 */
static constexpr const char *kThreadName = "thread.name";

/**
 * The method or function name, or equivalent (usually rightmost part of the code unit's name).
 */
static constexpr const char *kCodeFunction = "code.function";

/**
 * The &quot;namespace&quot; within which {@code code.function} is defined. Usually the qualified
 * class or module name, such that {@code code.namespace} + some separator + {@code code.function}
 * form a unique identifier for the code unit.
 */
static constexpr const char *kCodeNamespace = "code.namespace";

/**
 * The source code file name that identifies the code unit as uniquely as possible (preferably an
 * absolute file path).
 */
static constexpr const char *kCodeFilepath = "code.filepath";

/**
 * The line number in {@code code.filepath} best representing the operation. It SHOULD point within
 * the code unit named in {@code code.function}.
 */
static constexpr const char *kCodeLineno = "code.lineno";

/**
 * The column number in {@code code.filepath} best representing the operation. It SHOULD point
 * within the code unit named in {@code code.function}.
 */
static constexpr const char *kCodeColumn = "code.column";

/**
 * The size of the request payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size.
 */
static constexpr const char *kHttpRequestContentLength = "http.request_content_length";

/**
 * The size of the response payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://www.rfc-editor.org/rfc/rfc9110.html#field.content-length">Content-Length</a>
 * header. For requests using transport encoding, this should be the compressed size.
 */
static constexpr const char *kHttpResponseContentLength = "http.response_content_length";

/**
 * Full HTTP request URL in the form {@code scheme://host[:port]/path?query[#fragment]}. Usually the
 fragment is not transmitted over HTTP, but if it is known, it should be included nevertheless.
 *
 * <p>Notes:
  <ul> <li>{@code http.url} MUST NOT contain credentials passed via URL in form of {@code
 https://username:password@www.example.com/}. In such case the attribute's value should be {@code
 https://www.example.com/}.</li> </ul>
 */
static constexpr const char *kHttpUrl = "http.url";

/**
 * The ordinal number of request resending attempt (for any reason, including redirects).
 *
 * <p>Notes:
  <ul> <li>The resend count SHOULD be updated each time an HTTP request gets resent by the client,
 regardless of what was the cause of the resending (e.g. redirection, authorization failure, 503
 Server Unavailable, network issues, or any other).</li> </ul>
 */
static constexpr const char *kHttpResendCount = "http.resend_count";

/**
 * The full request target as passed in a HTTP request line or equivalent.
 */
static constexpr const char *kHttpTarget = "http.target";

/**
 * The IP address of the original client behind all proxies, if known (e.g. from <a
href="https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Forwarded-For">X-Forwarded-For</a>).
 *
 * <p>Notes:
  <ul> <li>This is not necessarily the same as {@code net.sock.peer.addr}, which would
identify the network-level peer, which may be a proxy.</li><li>This attribute should be set when a
source of information different from the one used for {@code net.sock.peer.addr}, is available even
if that other source just confirms the same value as {@code net.sock.peer.addr}. Rationale: For
{@code net.sock.peer.addr}, one typically does not know if it comes from a proxy, reverse proxy, or
the actual client. Setting
{@code http.client_ip} when it's the same as {@code net.sock.peer.addr} means that
one is at least somewhat confident that the address is not that of
the closest proxy.</li> </ul>
 */
static constexpr const char *kHttpClientIp = "http.client_ip";

/**
 * The AWS request ID as returned in the response headers {@code x-amz-request-id} or {@code
 * x-amz-requestid}.
 */
static constexpr const char *kAwsRequestId = "aws.request_id";

/**
 * The keys in the {@code RequestItems} object field.
 */
static constexpr const char *kAwsDynamodbTableNames = "aws.dynamodb.table_names";

/**
 * The JSON-serialized value of each item in the {@code ConsumedCapacity} response field.
 */
static constexpr const char *kAwsDynamodbConsumedCapacity = "aws.dynamodb.consumed_capacity";

/**
 * The JSON-serialized value of the {@code ItemCollectionMetrics} response field.
 */
static constexpr const char *kAwsDynamodbItemCollectionMetrics =
    "aws.dynamodb.item_collection_metrics";

/**
 * The value of the {@code ProvisionedThroughput.ReadCapacityUnits} request parameter.
 */
static constexpr const char *kAwsDynamodbProvisionedReadCapacity =
    "aws.dynamodb.provisioned_read_capacity";

/**
 * The value of the {@code ProvisionedThroughput.WriteCapacityUnits} request parameter.
 */
static constexpr const char *kAwsDynamodbProvisionedWriteCapacity =
    "aws.dynamodb.provisioned_write_capacity";

/**
 * The value of the {@code ConsistentRead} request parameter.
 */
static constexpr const char *kAwsDynamodbConsistentRead = "aws.dynamodb.consistent_read";

/**
 * The value of the {@code ProjectionExpression} request parameter.
 */
static constexpr const char *kAwsDynamodbProjection = "aws.dynamodb.projection";

/**
 * The value of the {@code Limit} request parameter.
 */
static constexpr const char *kAwsDynamodbLimit = "aws.dynamodb.limit";

/**
 * The value of the {@code AttributesToGet} request parameter.
 */
static constexpr const char *kAwsDynamodbAttributesToGet = "aws.dynamodb.attributes_to_get";

/**
 * The value of the {@code IndexName} request parameter.
 */
static constexpr const char *kAwsDynamodbIndexName = "aws.dynamodb.index_name";

/**
 * The value of the {@code Select} request parameter.
 */
static constexpr const char *kAwsDynamodbSelect = "aws.dynamodb.select";

/**
 * The JSON-serialized value of each item of the {@code GlobalSecondaryIndexes} request field
 */
static constexpr const char *kAwsDynamodbGlobalSecondaryIndexes =
    "aws.dynamodb.global_secondary_indexes";

/**
 * The JSON-serialized value of each item of the {@code LocalSecondaryIndexes} request field.
 */
static constexpr const char *kAwsDynamodbLocalSecondaryIndexes =
    "aws.dynamodb.local_secondary_indexes";

/**
 * The value of the {@code ExclusiveStartTableName} request parameter.
 */
static constexpr const char *kAwsDynamodbExclusiveStartTable = "aws.dynamodb.exclusive_start_table";

/**
 * The the number of items in the {@code TableNames} response parameter.
 */
static constexpr const char *kAwsDynamodbTableCount = "aws.dynamodb.table_count";

/**
 * The value of the {@code ScanIndexForward} request parameter.
 */
static constexpr const char *kAwsDynamodbScanForward = "aws.dynamodb.scan_forward";

/**
 * The value of the {@code Segment} request parameter.
 */
static constexpr const char *kAwsDynamodbSegment = "aws.dynamodb.segment";

/**
 * The value of the {@code TotalSegments} request parameter.
 */
static constexpr const char *kAwsDynamodbTotalSegments = "aws.dynamodb.total_segments";

/**
 * The value of the {@code Count} response parameter.
 */
static constexpr const char *kAwsDynamodbCount = "aws.dynamodb.count";

/**
 * The value of the {@code ScannedCount} response parameter.
 */
static constexpr const char *kAwsDynamodbScannedCount = "aws.dynamodb.scanned_count";

/**
 * The JSON-serialized value of each item in the {@code AttributeDefinitions} request field.
 */
static constexpr const char *kAwsDynamodbAttributeDefinitions =
    "aws.dynamodb.attribute_definitions";

/**
 * The JSON-serialized value of each item in the the {@code GlobalSecondaryIndexUpdates} request
 * field.
 */
static constexpr const char *kAwsDynamodbGlobalSecondaryIndexUpdates =
    "aws.dynamodb.global_secondary_index_updates";

/**
 * The S3 bucket name the request refers to. Corresponds to the {@code --bucket} parameter of the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/index.html">S3 API</a> operations.
 *
 * <p>Notes:
  <ul> <li>The {@code bucket} attribute is applicable to all S3 operations that reference a bucket,
i.e. that require the bucket name as a mandatory parameter. This applies to almost all S3 operations
except {@code list-buckets}.</li> </ul>
 */
static constexpr const char *kAwsS3Bucket = "aws.s3.bucket";

/**
 * The S3 object key the request refers to. Corresponds to the {@code --key} parameter of the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/index.html">S3 API</a> operations.
 *
 * <p>Notes:
  <ul> <li>The {@code key} attribute is applicable to all object-related S3 operations, i.e. that
require the object key as a mandatory parameter. This applies in particular to the following
operations:</li><li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/copy-object.html">copy-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/delete-object.html">delete-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/get-object.html">get-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/head-object.html">head-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/put-object.html">put-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/restore-object.html">restore-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/select-object-content.html">select-object-content</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/abort-multipart-upload.html">abort-multipart-upload</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/complete-multipart-upload.html">complete-multipart-upload</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/create-multipart-upload.html">create-multipart-upload</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/list-parts.html">list-parts</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part.html">upload-part</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part-copy.html">upload-part-copy</a></li>
 </ul>
 */
static constexpr const char *kAwsS3Key = "aws.s3.key";

/**
 * The source object (in the form {@code bucket}/{@code key}) for the copy operation.
 *
 * <p>Notes:
  <ul> <li>The {@code copy_source} attribute applies to S3 copy operations and corresponds to the
{@code --copy-source} parameter of the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/copy-object.html">copy-object operation
within the S3 API</a>. This applies in particular to the following operations:</li><li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/copy-object.html">copy-object</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part-copy.html">upload-part-copy</a></li>
 </ul>
 */
static constexpr const char *kAwsS3CopySource = "aws.s3.copy_source";

/**
 * Upload ID that identifies the multipart upload.
 *
 * <p>Notes:
  <ul> <li>The {@code upload_id} attribute applies to S3 multipart-upload operations and corresponds
to the {@code --upload-id} parameter of the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/index.html">S3 API</a> multipart
operations. This applies in particular to the following operations:</li><li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/abort-multipart-upload.html">abort-multipart-upload</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/complete-multipart-upload.html">complete-multipart-upload</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/list-parts.html">list-parts</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part.html">upload-part</a></li>
<li><a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part-copy.html">upload-part-copy</a></li>
 </ul>
 */
static constexpr const char *kAwsS3UploadId = "aws.s3.upload_id";

/**
 * The delete request container that specifies the objects to be deleted.
 *
 * <p>Notes:
  <ul> <li>The {@code delete} attribute is only applicable to the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/delete-object.html">delete-object</a>
operation. The {@code delete} attribute corresponds to the {@code --delete} parameter of the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/delete-objects.html">delete-objects
operation within the S3 API</a>.</li> </ul>
 */
static constexpr const char *kAwsS3Delete = "aws.s3.delete";

/**
 * The part number of the part being uploaded in a multipart-upload operation. This is a positive
integer between 1 and 10,000.
 *
 * <p>Notes:
  <ul> <li>The {@code part_number} attribute is only applicable to the <a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part.html">upload-part</a> and
<a
href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part-copy.html">upload-part-copy</a>
operations. The {@code part_number} attribute corresponds to the {@code --part-number} parameter of
the <a href="https://docs.aws.amazon.com/cli/latest/reference/s3api/upload-part.html">upload-part
operation within the S3 API</a>.</li> </ul>
 */
static constexpr const char *kAwsS3PartNumber = "aws.s3.part_number";

/**
 * The name of the operation being executed.
 */
static constexpr const char *kGraphqlOperationName = "graphql.operation.name";

/**
 * The type of the operation being executed.
 */
static constexpr const char *kGraphqlOperationType = "graphql.operation.type";

/**
 * The GraphQL document being executed.
 *
 * <p>Notes:
  <ul> <li>The value may be sanitized to exclude sensitive information.</li> </ul>
 */
static constexpr const char *kGraphqlDocument = "graphql.document";

/**
 * A value used by the messaging system as an identifier for the message, represented as a string.
 */
static constexpr const char *kMessagingMessageId = "messaging.message.id";

/**
 * The <a href="#conversations">conversation ID</a> identifying the conversation to which the
 * message belongs, represented as a string. Sometimes called &quot;Correlation ID&quot;.
 */
static constexpr const char *kMessagingMessageConversationId = "messaging.message.conversation_id";

/**
 * The (uncompressed) size of the message payload in bytes. Also use this attribute if it is unknown
 * whether the compressed or uncompressed payload size is reported.
 */
static constexpr const char *kMessagingMessagePayloadSizeBytes =
    "messaging.message.payload_size_bytes";

/**
 * The compressed size of the message payload in bytes.
 */
static constexpr const char *kMessagingMessagePayloadCompressedSizeBytes =
    "messaging.message.payload_compressed_size_bytes";

/**
 * The message destination name
 *
 * <p>Notes:
  <ul> <li>Destination name SHOULD uniquely identify a specific queue, topic or other entity within
the broker. If the broker does not have such notion, the destination name SHOULD uniquely identify
the broker.</li> </ul>
 */
static constexpr const char *kMessagingDestinationName = "messaging.destination.name";

/**
 * Low cardinality representation of the messaging destination name
 *
 * <p>Notes:
  <ul> <li>Destination names could be constructed from templates. An example would be a destination
 name involving a user name or product id. Although the destination name in this case is of high
 cardinality, the underlying template is of low cardinality and can be effectively used for grouping
 and aggregation.</li> </ul>
 */
static constexpr const char *kMessagingDestinationTemplate = "messaging.destination.template";

/**
 * A boolean that is true if the message destination is temporary and might not exist anymore after
 * messages are processed.
 */
static constexpr const char *kMessagingDestinationTemporary = "messaging.destination.temporary";

/**
 * A boolean that is true if the message destination is anonymous (could be unnamed or have
 * auto-generated name).
 */
static constexpr const char *kMessagingDestinationAnonymous = "messaging.destination.anonymous";

/**
 * The message source name
 *
 * <p>Notes:
  <ul> <li>Source name SHOULD uniquely identify a specific queue, topic, or other entity within the
broker. If the broker does not have such notion, the source name SHOULD uniquely identify the
broker.</li> </ul>
 */
static constexpr const char *kMessagingSourceName = "messaging.source.name";

/**
 * Low cardinality representation of the messaging source name
 *
 * <p>Notes:
  <ul> <li>Source names could be constructed from templates. An example would be a source name
 involving a user name or product id. Although the source name in this case is of high cardinality,
 the underlying template is of low cardinality and can be effectively used for grouping and
 aggregation.</li> </ul>
 */
static constexpr const char *kMessagingSourceTemplate = "messaging.source.template";

/**
 * A boolean that is true if the message source is temporary and might not exist anymore after
 * messages are processed.
 */
static constexpr const char *kMessagingSourceTemporary = "messaging.source.temporary";

/**
 * A boolean that is true if the message source is anonymous (could be unnamed or have
 * auto-generated name).
 */
static constexpr const char *kMessagingSourceAnonymous = "messaging.source.anonymous";

/**
 * A string identifying the messaging system.
 */
static constexpr const char *kMessagingSystem = "messaging.system";

/**
 * A string identifying the kind of messaging operation as defined in the <a
 href="#operation-names">Operation names</a> section above.
 *
 * <p>Notes:
  <ul> <li>If a custom value is used, it MUST be of low cardinality.</li> </ul>
 */
static constexpr const char *kMessagingOperation = "messaging.operation";

/**
 * The number of messages sent, received, or processed in the scope of the batching operation.
 *
 * <p>Notes:
  <ul> <li>Instrumentations SHOULD NOT set {@code messaging.batch.message_count} on spans that
 operate with a single message. When a messaging client library supports both batch and
 single-message API for the same operation, instrumentations SHOULD use {@code
 messaging.batch.message_count} for batching APIs and SHOULD NOT use it for single-message
 APIs.</li> </ul>
 */
static constexpr const char *kMessagingBatchMessageCount = "messaging.batch.message_count";

/**
 * The identifier for the consumer receiving a message. For Kafka, set it to {@code
 * {messaging.kafka.consumer.group} - {messaging.kafka.client_id}}, if both are present, or only
 * {@code messaging.kafka.consumer.group}. For brokers, such as RabbitMQ and Artemis, set it to the
 * {@code client_id} of the client consuming the message.
 */
static constexpr const char *kMessagingConsumerId = "messaging.consumer.id";

/**
 * RabbitMQ message routing key.
 */
static constexpr const char *kMessagingRabbitmqDestinationRoutingKey =
    "messaging.rabbitmq.destination.routing_key";

/**
 * Message keys in Kafka are used for grouping alike messages to ensure they're processed on the
 same partition. They differ from {@code messaging.message.id} in that they're not unique. If the
 key is {@code null}, the attribute MUST NOT be set.
 *
 * <p>Notes:
  <ul> <li>If the key type is not string, it's string representation has to be supplied for the
 attribute. If the key has no unambiguous, canonical string form, don't include its value.</li>
 </ul>
 */
static constexpr const char *kMessagingKafkaMessageKey = "messaging.kafka.message.key";

/**
 * Name of the Kafka Consumer Group that is handling the message. Only applies to consumers, not
 * producers.
 */
static constexpr const char *kMessagingKafkaConsumerGroup = "messaging.kafka.consumer.group";

/**
 * Client Id for the Consumer or Producer that is handling the message.
 */
static constexpr const char *kMessagingKafkaClientId = "messaging.kafka.client_id";

/**
 * Partition the message is sent to.
 */
static constexpr const char *kMessagingKafkaDestinationPartition =
    "messaging.kafka.destination.partition";

/**
 * Partition the message is received from.
 */
static constexpr const char *kMessagingKafkaSourcePartition = "messaging.kafka.source.partition";

/**
 * The offset of a record in the corresponding Kafka partition.
 */
static constexpr const char *kMessagingKafkaMessageOffset = "messaging.kafka.message.offset";

/**
 * A boolean that is true if the message is a tombstone.
 */
static constexpr const char *kMessagingKafkaMessageTombstone = "messaging.kafka.message.tombstone";

/**
 * Namespace of RocketMQ resources, resources in different namespaces are individual.
 */
static constexpr const char *kMessagingRocketmqNamespace = "messaging.rocketmq.namespace";

/**
 * Name of the RocketMQ producer/consumer group that is handling the message. The client type is
 * identified by the SpanKind.
 */
static constexpr const char *kMessagingRocketmqClientGroup = "messaging.rocketmq.client_group";

/**
 * The unique identifier for each client.
 */
static constexpr const char *kMessagingRocketmqClientId = "messaging.rocketmq.client_id";

/**
 * The timestamp in milliseconds that the delay message is expected to be delivered to consumer.
 */
static constexpr const char *kMessagingRocketmqMessageDeliveryTimestamp =
    "messaging.rocketmq.message.delivery_timestamp";

/**
 * The delay time level for delay message, which determines the message delay time.
 */
static constexpr const char *kMessagingRocketmqMessageDelayTimeLevel =
    "messaging.rocketmq.message.delay_time_level";

/**
 * It is essential for FIFO message. Messages that belong to the same message group are always
 * processed one by one within the same consumer group.
 */
static constexpr const char *kMessagingRocketmqMessageGroup = "messaging.rocketmq.message.group";

/**
 * Type of message.
 */
static constexpr const char *kMessagingRocketmqMessageType = "messaging.rocketmq.message.type";

/**
 * The secondary classifier of message besides topic.
 */
static constexpr const char *kMessagingRocketmqMessageTag = "messaging.rocketmq.message.tag";

/**
 * Key(s) of message, another way to mark message besides message id.
 */
static constexpr const char *kMessagingRocketmqMessageKeys = "messaging.rocketmq.message.keys";

/**
 * Model of message consumption. This only applies to consumer spans.
 */
static constexpr const char *kMessagingRocketmqConsumptionModel =
    "messaging.rocketmq.consumption_model";

/**
 * A string identifying the remoting system. See below for a list of well-known identifiers.
 */
static constexpr const char *kRpcSystem = "rpc.system";

/**
 * The full (logical) name of the service being called, including its package name, if applicable.
 *
 * <p>Notes:
  <ul> <li>This is the logical name of the service from the RPC interface perspective, which can be
 different from the name of any implementing class. The {@code code.namespace} attribute may be used
 to store the latter (despite the attribute name, it may include a class name; e.g., class with
 method actually executing the call on the server side, RPC client stub class on the client
 side).</li> </ul>
 */
static constexpr const char *kRpcService = "rpc.service";

/**
 * The name of the (logical) method being called, must be equal to the $method part in the span
 name.
 *
 * <p>Notes:
  <ul> <li>This is the logical name of the method from the RPC interface perspective, which can be
 different from the name of any implementing method/function. The {@code code.function} attribute
 may be used to store the latter (e.g., method actually executing the call on the server side, RPC
 client stub method on the client side).</li> </ul>
 */
static constexpr const char *kRpcMethod = "rpc.method";

/**
 * The <a href="https://github.com/grpc/grpc/blob/v1.33.2/doc/statuscodes.md">numeric status
 * code</a> of the gRPC request.
 */
static constexpr const char *kRpcGrpcStatusCode = "rpc.grpc.status_code";

/**
 * Protocol version as in {@code jsonrpc} property of request/response. Since JSON-RPC 1.0 does not
 * specify this, the value can be omitted.
 */
static constexpr const char *kRpcJsonrpcVersion = "rpc.jsonrpc.version";

/**
 * {@code id} property of request or response. Since protocol allows id to be int, string, {@code
 * null} or missing (for notifications), value is expected to be cast to string for simplicity. Use
 * empty string in case of {@code null} value. Omit entirely if this is a notification.
 */
static constexpr const char *kRpcJsonrpcRequestId = "rpc.jsonrpc.request_id";

/**
 * {@code error.code} property of response if it is an error response.
 */
static constexpr const char *kRpcJsonrpcErrorCode = "rpc.jsonrpc.error_code";

/**
 * {@code error.message} property of response if it is an error response.
 */
static constexpr const char *kRpcJsonrpcErrorMessage = "rpc.jsonrpc.error_message";

/**
 * Whether this is a received or sent message.
 */
static constexpr const char *kMessageType = "message.type";

/**
 * MUST be calculated as two different counters starting from {@code 1} one for sent messages and
 one for received message.
 *
 * <p>Notes:
  <ul> <li>This way we guarantee that the values will be consistent between different
 implementations.</li> </ul>
 */
static constexpr const char *kMessageId = "message.id";

/**
 * Compressed size of the message in bytes.
 */
static constexpr const char *kMessageCompressedSize = "message.compressed_size";

/**
 * Uncompressed size of the message in bytes.
 */
static constexpr const char *kMessageUncompressedSize = "message.uncompressed_size";

/**
 * The <a href="https://connect.build/docs/protocol/#error-codes">error codes</a> of the Connect
 * request. Error codes are always string values.
 */
static constexpr const char *kRpcConnectRpcErrorCode = "rpc.connect_rpc.error_code";

/**
 * SHOULD be set to true if the exception event is recorded at a point where it is known that the
exception is escaping the scope of the span.
 *
 * <p>Notes:
  <ul> <li>An exception is considered to have escaped (or left) the scope of a span,
if that span is ended while the exception is still logically &quot;in flight&quot;.
This may be actually &quot;in flight&quot; in some languages (e.g. if the exception
is passed to a Context manager's {@code __exit__} method in Python) but will
usually be caught at the point of recording the exception in most languages.</li><li>It is usually
not possible to determine at the point where an exception is thrown whether it will escape the scope
of a span. However, it is trivial to know that an exception will escape, if one checks for an active
exception just before ending the span, as done in the <a href="#recording-an-exception">example
above</a>.</li><li>It follows that an exception may still escape the scope of the span even if the
{@code exception.escaped} attribute was not set or set to false, since the event might have been
recorded at a time where it was not clear whether the exception will escape.</li> </ul>
 */
static constexpr const char *kExceptionEscaped = "exception.escaped";

/**
 * Value of the <a href="https://www.rfc-editor.org/rfc/rfc9110.html#field.user-agent">HTTP
 * User-Agent</a> header sent by the client.
 */
static constexpr const char *kUserAgentOriginal = "user_agent.original";

// Enum definitions
namespace EventDomainValues
{
/** Events from browser apps. */
static constexpr const char *kBrowser = "browser";
/** Events from mobile apps. */
static constexpr const char *kDevice = "device";
/** Events from Kubernetes. */
static constexpr const char *kK8s = "k8s";
}  // namespace EventDomainValues

namespace OpentracingRefTypeValues
{
/** The parent Span depends on the child Span in some capacity. */
static constexpr const char *kChildOf = "child_of";
/** The parent Span does not depend in any way on the result of the child Span. */
static constexpr const char *kFollowsFrom = "follows_from";
}  // namespace OpentracingRefTypeValues

namespace DbSystemValues
{
/** Some other SQL database. Fallback only. See notes. */
static constexpr const char *kOtherSql = "other_sql";
/** Microsoft SQL Server. */
static constexpr const char *kMssql = "mssql";
/** Microsoft SQL Server Compact. */
static constexpr const char *kMssqlcompact = "mssqlcompact";
/** MySQL. */
static constexpr const char *kMysql = "mysql";
/** Oracle Database. */
static constexpr const char *kOracle = "oracle";
/** IBM Db2. */
static constexpr const char *kDb2 = "db2";
/** PostgreSQL. */
static constexpr const char *kPostgresql = "postgresql";
/** Amazon Redshift. */
static constexpr const char *kRedshift = "redshift";
/** Apache Hive. */
static constexpr const char *kHive = "hive";
/** Cloudscape. */
static constexpr const char *kCloudscape = "cloudscape";
/** HyperSQL DataBase. */
static constexpr const char *kHsqldb = "hsqldb";
/** Progress Database. */
static constexpr const char *kProgress = "progress";
/** SAP MaxDB. */
static constexpr const char *kMaxdb = "maxdb";
/** SAP HANA. */
static constexpr const char *kHanadb = "hanadb";
/** Ingres. */
static constexpr const char *kIngres = "ingres";
/** FirstSQL. */
static constexpr const char *kFirstsql = "firstsql";
/** EnterpriseDB. */
static constexpr const char *kEdb = "edb";
/** InterSystems Caché. */
static constexpr const char *kCache = "cache";
/** Adabas (Adaptable Database System). */
static constexpr const char *kAdabas = "adabas";
/** Firebird. */
static constexpr const char *kFirebird = "firebird";
/** Apache Derby. */
static constexpr const char *kDerby = "derby";
/** FileMaker. */
static constexpr const char *kFilemaker = "filemaker";
/** Informix. */
static constexpr const char *kInformix = "informix";
/** InstantDB. */
static constexpr const char *kInstantdb = "instantdb";
/** InterBase. */
static constexpr const char *kInterbase = "interbase";
/** MariaDB. */
static constexpr const char *kMariadb = "mariadb";
/** Netezza. */
static constexpr const char *kNetezza = "netezza";
/** Pervasive PSQL. */
static constexpr const char *kPervasive = "pervasive";
/** PointBase. */
static constexpr const char *kPointbase = "pointbase";
/** SQLite. */
static constexpr const char *kSqlite = "sqlite";
/** Sybase. */
static constexpr const char *kSybase = "sybase";
/** Teradata. */
static constexpr const char *kTeradata = "teradata";
/** Vertica. */
static constexpr const char *kVertica = "vertica";
/** H2. */
static constexpr const char *kH2 = "h2";
/** ColdFusion IMQ. */
static constexpr const char *kColdfusion = "coldfusion";
/** Apache Cassandra. */
static constexpr const char *kCassandra = "cassandra";
/** Apache HBase. */
static constexpr const char *kHbase = "hbase";
/** MongoDB. */
static constexpr const char *kMongodb = "mongodb";
/** Redis. */
static constexpr const char *kRedis = "redis";
/** Couchbase. */
static constexpr const char *kCouchbase = "couchbase";
/** CouchDB. */
static constexpr const char *kCouchdb = "couchdb";
/** Microsoft Azure Cosmos DB. */
static constexpr const char *kCosmosdb = "cosmosdb";
/** Amazon DynamoDB. */
static constexpr const char *kDynamodb = "dynamodb";
/** Neo4j. */
static constexpr const char *kNeo4j = "neo4j";
/** Apache Geode. */
static constexpr const char *kGeode = "geode";
/** Elasticsearch. */
static constexpr const char *kElasticsearch = "elasticsearch";
/** Memcached. */
static constexpr const char *kMemcached = "memcached";
/** CockroachDB. */
static constexpr const char *kCockroachdb = "cockroachdb";
/** OpenSearch. */
static constexpr const char *kOpensearch = "opensearch";
/** ClickHouse. */
static constexpr const char *kClickhouse = "clickhouse";
/** Cloud Spanner. */
static constexpr const char *kSpanner = "spanner";
/** Trino. */
static constexpr const char *kTrino = "trino";
}  // namespace DbSystemValues

namespace DbCassandraConsistencyLevelValues
{
/** all. */
static constexpr const char *kAll = "all";
/** each_quorum. */
static constexpr const char *kEachQuorum = "each_quorum";
/** quorum. */
static constexpr const char *kQuorum = "quorum";
/** local_quorum. */
static constexpr const char *kLocalQuorum = "local_quorum";
/** one. */
static constexpr const char *kOne = "one";
/** two. */
static constexpr const char *kTwo = "two";
/** three. */
static constexpr const char *kThree = "three";
/** local_one. */
static constexpr const char *kLocalOne = "local_one";
/** any. */
static constexpr const char *kAny = "any";
/** serial. */
static constexpr const char *kSerial = "serial";
/** local_serial. */
static constexpr const char *kLocalSerial = "local_serial";
}  // namespace DbCassandraConsistencyLevelValues

namespace DbCosmosdbOperationTypeValues
{
/** invalid. */
static constexpr const char *kInvalid = "Invalid";
/** create. */
static constexpr const char *kCreate = "Create";
/** patch. */
static constexpr const char *kPatch = "Patch";
/** read. */
static constexpr const char *kRead = "Read";
/** read_feed. */
static constexpr const char *kReadFeed = "ReadFeed";
/** delete. */
static constexpr const char *kDelete = "Delete";
/** replace. */
static constexpr const char *kReplace = "Replace";
/** execute. */
static constexpr const char *kExecute = "Execute";
/** query. */
static constexpr const char *kQuery = "Query";
/** head. */
static constexpr const char *kHead = "Head";
/** head_feed. */
static constexpr const char *kHeadFeed = "HeadFeed";
/** upsert. */
static constexpr const char *kUpsert = "Upsert";
/** batch. */
static constexpr const char *kBatch = "Batch";
/** query_plan. */
static constexpr const char *kQueryPlan = "QueryPlan";
/** execute_javascript. */
static constexpr const char *kExecuteJavascript = "ExecuteJavaScript";
}  // namespace DbCosmosdbOperationTypeValues

namespace DbCosmosdbConnectionModeValues
{
/** Gateway (HTTP) connections mode. */
static constexpr const char *kGateway = "gateway";
/** Direct connection. */
static constexpr const char *kDirect = "direct";
}  // namespace DbCosmosdbConnectionModeValues

namespace OtelStatusCodeValues
{
/** The operation has been validated by an Application developer or Operator to have completed
 * successfully. */
static constexpr const char *kOk = "OK";
/** The operation contains an error. */
static constexpr const char *kError = "ERROR";
}  // namespace OtelStatusCodeValues

namespace FaasTriggerValues
{
/** A response to some data source operation such as a database or filesystem read/write. */
static constexpr const char *kDatasource = "datasource";
/** To provide an answer to an inbound HTTP request. */
static constexpr const char *kHttp = "http";
/** A function is set to be executed when messages are sent to a messaging system. */
static constexpr const char *kPubsub = "pubsub";
/** A function is scheduled to be executed regularly. */
static constexpr const char *kTimer = "timer";
/** If none of the others apply. */
static constexpr const char *kOther = "other";
}  // namespace FaasTriggerValues

namespace FaasDocumentOperationValues
{
/** When a new object is created. */
static constexpr const char *kInsert = "insert";
/** When an object is modified. */
static constexpr const char *kEdit = "edit";
/** When an object is deleted. */
static constexpr const char *kDelete = "delete";
}  // namespace FaasDocumentOperationValues

namespace FaasInvokedProviderValues
{
/** Alibaba Cloud. */
static constexpr const char *kAlibabaCloud = "alibaba_cloud";
/** Amazon Web Services. */
static constexpr const char *kAws = "aws";
/** Microsoft Azure. */
static constexpr const char *kAzure = "azure";
/** Google Cloud Platform. */
static constexpr const char *kGcp = "gcp";
/** Tencent Cloud. */
static constexpr const char *kTencentCloud = "tencent_cloud";
}  // namespace FaasInvokedProviderValues

namespace NetTransportValues
{
/** ip_tcp. */
static constexpr const char *kIpTcp = "ip_tcp";
/** ip_udp. */
static constexpr const char *kIpUdp = "ip_udp";
/** Named or anonymous pipe. See note below. */
static constexpr const char *kPipe = "pipe";
/** In-process communication. */
static constexpr const char *kInproc = "inproc";
/** Something else (non IP-based). */
static constexpr const char *kOther = "other";
}  // namespace NetTransportValues

namespace NetSockFamilyValues
{
/** IPv4 address. */
static constexpr const char *kInet = "inet";
/** IPv6 address. */
static constexpr const char *kInet6 = "inet6";
/** Unix domain socket path. */
static constexpr const char *kUnix = "unix";
}  // namespace NetSockFamilyValues

namespace NetHostConnectionTypeValues
{
/** wifi. */
static constexpr const char *kWifi = "wifi";
/** wired. */
static constexpr const char *kWired = "wired";
/** cell. */
static constexpr const char *kCell = "cell";
/** unavailable. */
static constexpr const char *kUnavailable = "unavailable";
/** unknown. */
static constexpr const char *kUnknown = "unknown";
}  // namespace NetHostConnectionTypeValues

namespace NetHostConnectionSubtypeValues
{
/** GPRS. */
static constexpr const char *kGprs = "gprs";
/** EDGE. */
static constexpr const char *kEdge = "edge";
/** UMTS. */
static constexpr const char *kUmts = "umts";
/** CDMA. */
static constexpr const char *kCdma = "cdma";
/** EVDO Rel. 0. */
static constexpr const char *kEvdo0 = "evdo_0";
/** EVDO Rev. A. */
static constexpr const char *kEvdoA = "evdo_a";
/** CDMA2000 1XRTT. */
static constexpr const char *kCdma20001xrtt = "cdma2000_1xrtt";
/** HSDPA. */
static constexpr const char *kHsdpa = "hsdpa";
/** HSUPA. */
static constexpr const char *kHsupa = "hsupa";
/** HSPA. */
static constexpr const char *kHspa = "hspa";
/** IDEN. */
static constexpr const char *kIden = "iden";
/** EVDO Rev. B. */
static constexpr const char *kEvdoB = "evdo_b";
/** LTE. */
static constexpr const char *kLte = "lte";
/** EHRPD. */
static constexpr const char *kEhrpd = "ehrpd";
/** HSPAP. */
static constexpr const char *kHspap = "hspap";
/** GSM. */
static constexpr const char *kGsm = "gsm";
/** TD-SCDMA. */
static constexpr const char *kTdScdma = "td_scdma";
/** IWLAN. */
static constexpr const char *kIwlan = "iwlan";
/** 5G NR (New Radio). */
static constexpr const char *kNr = "nr";
/** 5G NRNSA (New Radio Non-Standalone). */
static constexpr const char *kNrnsa = "nrnsa";
/** LTE CA. */
static constexpr const char *kLteCa = "lte_ca";
}  // namespace NetHostConnectionSubtypeValues

namespace GraphqlOperationTypeValues
{
/** GraphQL query. */
static constexpr const char *kQuery = "query";
/** GraphQL mutation. */
static constexpr const char *kMutation = "mutation";
/** GraphQL subscription. */
static constexpr const char *kSubscription = "subscription";
}  // namespace GraphqlOperationTypeValues

namespace MessagingOperationValues
{
/** publish. */
static constexpr const char *kPublish = "publish";
/** receive. */
static constexpr const char *kReceive = "receive";
/** process. */
static constexpr const char *kProcess = "process";
}  // namespace MessagingOperationValues

namespace MessagingRocketmqMessageTypeValues
{
/** Normal message. */
static constexpr const char *kNormal = "normal";
/** FIFO message. */
static constexpr const char *kFifo = "fifo";
/** Delay message. */
static constexpr const char *kDelay = "delay";
/** Transaction message. */
static constexpr const char *kTransaction = "transaction";
}  // namespace MessagingRocketmqMessageTypeValues

namespace MessagingRocketmqConsumptionModelValues
{
/** Clustering consumption model. */
static constexpr const char *kClustering = "clustering";
/** Broadcasting consumption model. */
static constexpr const char *kBroadcasting = "broadcasting";
}  // namespace MessagingRocketmqConsumptionModelValues

namespace RpcSystemValues
{
/** gRPC. */
static constexpr const char *kGrpc = "grpc";
/** Java RMI. */
static constexpr const char *kJavaRmi = "java_rmi";
/** .NET WCF. */
static constexpr const char *kDotnetWcf = "dotnet_wcf";
/** Apache Dubbo. */
static constexpr const char *kApacheDubbo = "apache_dubbo";
/** Connect RPC. */
static constexpr const char *kConnectRpc = "connect_rpc";
}  // namespace RpcSystemValues

namespace RpcGrpcStatusCodeValues
{
/** OK. */
static constexpr const int kOk = 0;
/** CANCELLED. */
static constexpr const int kCancelled = 1;
/** UNKNOWN. */
static constexpr const int kUnknown = 2;
/** INVALID_ARGUMENT. */
static constexpr const int kInvalidArgument = 3;
/** DEADLINE_EXCEEDED. */
static constexpr const int kDeadlineExceeded = 4;
/** NOT_FOUND. */
static constexpr const int kNotFound = 5;
/** ALREADY_EXISTS. */
static constexpr const int kAlreadyExists = 6;
/** PERMISSION_DENIED. */
static constexpr const int kPermissionDenied = 7;
/** RESOURCE_EXHAUSTED. */
static constexpr const int kResourceExhausted = 8;
/** FAILED_PRECONDITION. */
static constexpr const int kFailedPrecondition = 9;
/** ABORTED. */
static constexpr const int kAborted = 10;
/** OUT_OF_RANGE. */
static constexpr const int kOutOfRange = 11;
/** UNIMPLEMENTED. */
static constexpr const int kUnimplemented = 12;
/** INTERNAL. */
static constexpr const int kInternal = 13;
/** UNAVAILABLE. */
static constexpr const int kUnavailable = 14;
/** DATA_LOSS. */
static constexpr const int kDataLoss = 15;
/** UNAUTHENTICATED. */
static constexpr const int kUnauthenticated = 16;
}  // namespace RpcGrpcStatusCodeValues

namespace MessageTypeValues
{
/** sent. */
static constexpr const char *kSent = "SENT";
/** received. */
static constexpr const char *kReceived = "RECEIVED";
}  // namespace MessageTypeValues

namespace RpcConnectRpcErrorCodeValues
{
/** cancelled. */
static constexpr const char *kCancelled = "cancelled";
/** unknown. */
static constexpr const char *kUnknown = "unknown";
/** invalid_argument. */
static constexpr const char *kInvalidArgument = "invalid_argument";
/** deadline_exceeded. */
static constexpr const char *kDeadlineExceeded = "deadline_exceeded";
/** not_found. */
static constexpr const char *kNotFound = "not_found";
/** already_exists. */
static constexpr const char *kAlreadyExists = "already_exists";
/** permission_denied. */
static constexpr const char *kPermissionDenied = "permission_denied";
/** resource_exhausted. */
static constexpr const char *kResourceExhausted = "resource_exhausted";
/** failed_precondition. */
static constexpr const char *kFailedPrecondition = "failed_precondition";
/** aborted. */
static constexpr const char *kAborted = "aborted";
/** out_of_range. */
static constexpr const char *kOutOfRange = "out_of_range";
/** unimplemented. */
static constexpr const char *kUnimplemented = "unimplemented";
/** internal. */
static constexpr const char *kInternal = "internal";
/** unavailable. */
static constexpr const char *kUnavailable = "unavailable";
/** data_loss. */
static constexpr const char *kDataLoss = "data_loss";
/** unauthenticated. */
static constexpr const char *kUnauthenticated = "unauthenticated";
}  // namespace RpcConnectRpcErrorCodeValues

}  // namespace SemanticConventions
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
