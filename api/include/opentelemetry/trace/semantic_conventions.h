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
/*
 * Special considerations for FaasDocumentOperationValues::DELETE.
 *
 * The winnt.h header file is known to define a DELETE symbol,
 * which collides with FaasDocumentOperationValues::DELETE.
 */
#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": removing define on DELETE")
#    undef DELETE
#  endif
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

namespace SemanticConventions
{
/**
 * The URL of the OpenTelemetry schema for these keys and values.
 */
static constexpr const char *SCHEMA_URL = "https://opentelemetry.io/schemas/1.12.0";

/**
 * The full invoked ARN as provided on the {@code Context} passed to the function ({@code
 Lambda-Runtime-Invoked-Function-Arn} header on the {@code /runtime/invocation/next} applicable).
 *
 * <p>Notes:
  <ul> <li>This may be different from {@code faas.id} if an alias is involved.</li> </ul>
 */
static constexpr const char *AWS_LAMBDA_INVOKED_ARN = "aws.lambda.invoked_arn";

/**
 * The <a href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#id">event_id</a>
 * uniquely identifies the event.
 */
static constexpr const char *CLOUDEVENTS_EVENT_ID = "cloudevents.event_id";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#source-1">source</a>
 * identifies the context in which an event happened.
 */
static constexpr const char *CLOUDEVENTS_EVENT_SOURCE = "cloudevents.event_source";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#specversion">version of
 * the CloudEvents specification</a> which the event uses.
 */
static constexpr const char *CLOUDEVENTS_EVENT_SPEC_VERSION = "cloudevents.event_spec_version";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#type">event_type</a>
 * contains a value describing the type of event related to the originating occurrence.
 */
static constexpr const char *CLOUDEVENTS_EVENT_TYPE = "cloudevents.event_type";

/**
 * The <a
 * href="https://github.com/cloudevents/spec/blob/v1.0.2/cloudevents/spec.md#subject">subject</a> of
 * the event in the context of the event producer (identified by source).
 */
static constexpr const char *CLOUDEVENTS_EVENT_SUBJECT = "cloudevents.event_subject";

/**
 * Parent-child Reference type
 *
 * <p>Notes:
  <ul> <li>The causal relationship between a child Span and a parent Span.</li> </ul>
 */
static constexpr const char *OPENTRACING_REF_TYPE = "opentracing.ref_type";

/**
 * An identifier for the database management system (DBMS) product being used. See below for a list
 * of well-known identifiers.
 */
static constexpr const char *DB_SYSTEM = "db.system";

/**
 * The connection string used to connect to the database. It is recommended to remove embedded
 * credentials.
 */
static constexpr const char *DB_CONNECTION_STRING = "db.connection_string";

/**
 * Username for accessing the database.
 */
static constexpr const char *DB_USER = "db.user";

/**
 * The fully-qualified class name of the <a
 * href="https://docs.oracle.com/javase/8/docs/technotes/guides/jdbc/">Java Database Connectivity
 * (JDBC)</a> driver used to connect.
 */
static constexpr const char *DB_JDBC_DRIVER_CLASSNAME = "db.jdbc.driver_classname";

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
static constexpr const char *DB_NAME = "db.name";

/**
 * The database statement being executed.
 *
 * <p>Notes:
  <ul> <li>The value may be sanitized to exclude sensitive information.</li> </ul>
 */
static constexpr const char *DB_STATEMENT = "db.statement";

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
static constexpr const char *DB_OPERATION = "db.operation";

/**
 * The Microsoft SQL Server <a
 href="https://docs.microsoft.com/en-us/sql/connect/jdbc/building-the-connection-url?view=sql-server-ver15">instance
 name</a> connecting to. This name is used to determine the port of a named instance.
 *
 * <p>Notes:
  <ul> <li>If setting a {@code db.mssql.instance_name}, {@code net.peer.port} is no longer required
 (but still recommended if non-standard).</li> </ul>
 */
static constexpr const char *DB_MSSQL_INSTANCE_NAME = "db.mssql.instance_name";

/**
 * The fetch size used for paging, i.e. how many rows will be returned at once.
 */
static constexpr const char *DB_CASSANDRA_PAGE_SIZE = "db.cassandra.page_size";

/**
 * The consistency level of the query. Based on consistency values from <a
 * href="https://docs.datastax.com/en/cassandra-oss/3.0/cassandra/dml/dmlConfigConsistency.html">CQL</a>.
 */
static constexpr const char *DB_CASSANDRA_CONSISTENCY_LEVEL = "db.cassandra.consistency_level";

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
static constexpr const char *DB_CASSANDRA_TABLE = "db.cassandra.table";

/**
 * Whether or not the query is idempotent.
 */
static constexpr const char *DB_CASSANDRA_IDEMPOTENCE = "db.cassandra.idempotence";

/**
 * The number of times a query was speculatively executed. Not set or {@code 0} if the query was not
 * executed speculatively.
 */
static constexpr const char *DB_CASSANDRA_SPECULATIVE_EXECUTION_COUNT =
    "db.cassandra.speculative_execution_count";

/**
 * The ID of the coordinating node for a query.
 */
static constexpr const char *DB_CASSANDRA_COORDINATOR_ID = "db.cassandra.coordinator.id";

/**
 * The data center of the coordinating node for a query.
 */
static constexpr const char *DB_CASSANDRA_COORDINATOR_DC = "db.cassandra.coordinator.dc";

/**
 * The index of the database being accessed as used in the <a
 * href="https://redis.io/commands/select">{@code SELECT} command</a>, provided as an integer. To be
 * used instead of the generic {@code db.name} attribute.
 */
static constexpr const char *DB_REDIS_DATABASE_INDEX = "db.redis.database_index";

/**
 * The collection being accessed within the database stated in {@code db.name}.
 */
static constexpr const char *DB_MONGODB_COLLECTION = "db.mongodb.collection";

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
static constexpr const char *DB_SQL_TABLE = "db.sql.table";

/**
 * The type of the exception (its fully-qualified class name, if applicable). The dynamic type of
 * the exception should be preferred over the static type in languages that support it.
 */
static constexpr const char *EXCEPTION_TYPE = "exception.type";

/**
 * The exception message.
 */
static constexpr const char *EXCEPTION_MESSAGE = "exception.message";

/**
 * A stacktrace as a string in the natural representation for the language runtime. The
 * representation is to be determined and documented by each language SIG.
 */
static constexpr const char *EXCEPTION_STACKTRACE = "exception.stacktrace";

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
static constexpr const char *EXCEPTION_ESCAPED = "exception.escaped";

/**
 * Type of the trigger which caused this function execution.
 *
 * <p>Notes:
  <ul> <li>For the server/consumer span on the incoming side,
{@code faas.trigger} MUST be set.</li><li>Clients invoking FaaS instances usually cannot set {@code
faas.trigger}, since they would typically need to look in the payload to determine the event type.
If clients set it, it should be the same as the trigger that corresponding incoming would have
(i.e., this has nothing to do with the underlying transport used to make the API call to invoke the
lambda, which is often HTTP).</li> </ul>
 */
static constexpr const char *FAAS_TRIGGER = "faas.trigger";

/**
 * The execution ID of the current function execution.
 */
static constexpr const char *FAAS_EXECUTION = "faas.execution";

/**
 * The name of the source on which the triggering operation was performed. For example, in Cloud
 * Storage or S3 corresponds to the bucket name, and in Cosmos DB to the database name.
 */
static constexpr const char *FAAS_DOCUMENT_COLLECTION = "faas.document.collection";

/**
 * Describes the type of the operation that was performed on the data.
 */
static constexpr const char *FAAS_DOCUMENT_OPERATION = "faas.document.operation";

/**
 * A string containing the time when the data was accessed in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static constexpr const char *FAAS_DOCUMENT_TIME = "faas.document.time";

/**
 * The document name/table subjected to the operation. For example, in Cloud Storage or S3 is the
 * name of the file, and in Cosmos DB the table name.
 */
static constexpr const char *FAAS_DOCUMENT_NAME = "faas.document.name";

/**
 * A string containing the function invocation time in the <a
 * href="https://www.iso.org/iso-8601-date-and-time-format.html">ISO 8601</a> format expressed in <a
 * href="https://www.w3.org/TR/NOTE-datetime">UTC</a>.
 */
static constexpr const char *FAAS_TIME = "faas.time";

/**
 * A string containing the schedule period as <a
 * href="https://docs.oracle.com/cd/E12058_01/doc/doc.1014/e12030/cron_expressions.htm">Cron
 * Expression</a>.
 */
static constexpr const char *FAAS_CRON = "faas.cron";

/**
 * A boolean that is true if the serverless function is executed for the first time (aka
 * cold-start).
 */
static constexpr const char *FAAS_COLDSTART = "faas.coldstart";

/**
 * The name of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code faas.name} resource attribute of the invoked function.</li>
 </ul>
 */
static constexpr const char *FAAS_INVOKED_NAME = "faas.invoked_name";

/**
 * The cloud provider of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code cloud.provider} resource attribute of the invoked
 function.</li> </ul>
 */
static constexpr const char *FAAS_INVOKED_PROVIDER = "faas.invoked_provider";

/**
 * The cloud region of the invoked function.
 *
 * <p>Notes:
  <ul> <li>SHOULD be equal to the {@code cloud.region} resource attribute of the invoked
 function.</li> </ul>
 */
static constexpr const char *FAAS_INVOKED_REGION = "faas.invoked_region";

/**
 * Transport protocol used. See note below.
 */
static constexpr const char *NET_TRANSPORT = "net.transport";

/**
 * Remote address of the peer (dotted decimal for IPv4 or <a
 * href="https://tools.ietf.org/html/rfc5952">RFC5952</a> for IPv6)
 */
static constexpr const char *NET_PEER_IP = "net.peer.ip";

/**
 * Remote port number.
 */
static constexpr const char *NET_PEER_PORT = "net.peer.port";

/**
 * Remote hostname or similar, see note below.
 *
 * <p>Notes:
  <ul> <li>{@code net.peer.name} SHOULD NOT be set if capturing it would require an extra DNS
 lookup.</li> </ul>
 */
static constexpr const char *NET_PEER_NAME = "net.peer.name";

/**
 * Like {@code net.peer.ip} but for the host IP. Useful in case of a multi-IP host.
 */
static constexpr const char *NET_HOST_IP = "net.host.ip";

/**
 * Like {@code net.peer.port} but for the host port.
 */
static constexpr const char *NET_HOST_PORT = "net.host.port";

/**
 * Local hostname or similar, see note below.
 */
static constexpr const char *NET_HOST_NAME = "net.host.name";

/**
 * The internet connection type currently being used by the host.
 */
static constexpr const char *NET_HOST_CONNECTION_TYPE = "net.host.connection.type";

/**
 * This describes more details regarding the connection.type. It may be the type of cell technology
 * connection, but it could be used for describing details about a wifi connection.
 */
static constexpr const char *NET_HOST_CONNECTION_SUBTYPE = "net.host.connection.subtype";

/**
 * The name of the mobile carrier.
 */
static constexpr const char *NET_HOST_CARRIER_NAME = "net.host.carrier.name";

/**
 * The mobile carrier country code.
 */
static constexpr const char *NET_HOST_CARRIER_MCC = "net.host.carrier.mcc";

/**
 * The mobile carrier network code.
 */
static constexpr const char *NET_HOST_CARRIER_MNC = "net.host.carrier.mnc";

/**
 * The ISO 3166-1 alpha-2 2-character country code associated with the mobile carrier network.
 */
static constexpr const char *NET_HOST_CARRIER_ICC = "net.host.carrier.icc";

/**
 * The <a href="../../resource/semantic_conventions/README.md#service">{@code service.name}</a> of
 * the remote service. SHOULD be equal to the actual {@code service.name} resource attribute of the
 * remote service if any.
 */
static constexpr const char *PEER_SERVICE = "peer.service";

/**
 * Username or client_id extracted from the access token or <a
 * href="https://tools.ietf.org/html/rfc7235#section-4.2">Authorization</a> header in the inbound
 * request from outside the system.
 */
static constexpr const char *ENDUSER_ID = "enduser.id";

/**
 * Actual/assumed role the client is making the request under extracted from token or application
 * security context.
 */
static constexpr const char *ENDUSER_ROLE = "enduser.role";

/**
 * Scopes or granted authorities the client currently possesses extracted from token or application
 * security context. The value would come from the scope associated with an <a
 * href="https://tools.ietf.org/html/rfc6749#section-3.3">OAuth 2.0 Access Token</a> or an attribute
 * value in a <a
 * href="http://docs.oasis-open.org/security/saml/Post2.0/sstc-saml-tech-overview-2.0.html">SAML 2.0
 * Assertion</a>.
 */
static constexpr const char *ENDUSER_SCOPE = "enduser.scope";

/**
 * Current &quot;managed&quot; thread ID (as opposed to OS thread ID).
 */
static constexpr const char *THREAD_ID = "thread.id";

/**
 * Current thread name.
 */
static constexpr const char *THREAD_NAME = "thread.name";

/**
 * The method or function name, or equivalent (usually rightmost part of the code unit's name).
 */
static constexpr const char *CODE_FUNCTION = "code.function";

/**
 * The &quot;namespace&quot; within which {@code code.function} is defined. Usually the qualified
 * class or module name, such that {@code code.namespace} + some separator + {@code code.function}
 * form a unique identifier for the code unit.
 */
static constexpr const char *CODE_NAMESPACE = "code.namespace";

/**
 * The source code file name that identifies the code unit as uniquely as possible (preferably an
 * absolute file path).
 */
static constexpr const char *CODE_FILEPATH = "code.filepath";

/**
 * The line number in {@code code.filepath} best representing the operation. It SHOULD point within
 * the code unit named in {@code code.function}.
 */
static constexpr const char *CODE_LINENO = "code.lineno";

/**
 * HTTP request method.
 */
static constexpr const char *HTTP_METHOD = "http.method";

/**
 * Full HTTP request URL in the form {@code scheme://host[:port]/path?query[#fragment]}. Usually the
 fragment is not transmitted over HTTP, but if it is known, it should be included nevertheless.
 *
 * <p>Notes:
  <ul> <li>{@code http.url} MUST NOT contain credentials passed via URL in form of {@code
 https://username:password@www.example.com/}. In such case the attribute's value should be {@code
 https://www.example.com/}.</li> </ul>
 */
static constexpr const char *HTTP_URL = "http.url";

/**
 * The full request target as passed in a HTTP request line or equivalent.
 */
static constexpr const char *HTTP_TARGET = "http.target";

/**
 * The value of the <a href="https://tools.ietf.org/html/rfc7230#section-5.4">HTTP host header</a>.
 An empty Host header should also be reported, see note.
 *
 * <p>Notes:
  <ul> <li>When the header is present but empty the attribute SHOULD be set to the empty string.
 Note that this is a valid situation that is expected in certain cases, according the aforementioned
 <a href="https://tools.ietf.org/html/rfc7230#section-5.4">section of RFC 7230</a>. When the header
 is not set the attribute MUST NOT be set.</li> </ul>
 */
static constexpr const char *HTTP_HOST = "http.host";

/**
 * The URI scheme identifying the used protocol.
 */
static constexpr const char *HTTP_SCHEME = "http.scheme";

/**
 * <a href="https://tools.ietf.org/html/rfc7231#section-6">HTTP response status code</a>.
 */
static constexpr const char *HTTP_STATUS_CODE = "http.status_code";

/**
 * Kind of HTTP protocol used.
 *
 * <p>Notes:
  <ul> <li>If {@code net.transport} is not specified, it can be assumed to be {@code IP.TCP} except
 if {@code http.flavor} is {@code QUIC}, in which case {@code IP.UDP} is assumed.</li> </ul>
 */
static constexpr const char *HTTP_FLAVOR = "http.flavor";

/**
 * Value of the <a href="https://tools.ietf.org/html/rfc7231#section-5.5.3">HTTP User-Agent</a>
 * header sent by the client.
 */
static constexpr const char *HTTP_USER_AGENT = "http.user_agent";

/**
 * The size of the request payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://tools.ietf.org/html/rfc7230#section-3.3.2">Content-Length</a> header. For requests
 * using transport encoding, this should be the compressed size.
 */
static constexpr const char *HTTP_REQUEST_CONTENT_LENGTH = "http.request_content_length";

/**
 * The size of the uncompressed request payload body after transport decoding. Not set if transport
 * encoding not used.
 */
static constexpr const char *HTTP_REQUEST_CONTENT_LENGTH_UNCOMPRESSED =
    "http.request_content_length_uncompressed";

/**
 * The size of the response payload body in bytes. This is the number of bytes transferred excluding
 * headers and is often, but not always, present as the <a
 * href="https://tools.ietf.org/html/rfc7230#section-3.3.2">Content-Length</a> header. For requests
 * using transport encoding, this should be the compressed size.
 */
static constexpr const char *HTTP_RESPONSE_CONTENT_LENGTH = "http.response_content_length";

/**
 * The size of the uncompressed response payload body after transport decoding. Not set if transport
 * encoding not used.
 */
static constexpr const char *HTTP_RESPONSE_CONTENT_LENGTH_UNCOMPRESSED =
    "http.response_content_length_uncompressed";

/**
 * The ordinal number of request re-sending attempt.
 */
static constexpr const char *HTTP_RETRY_COUNT = "http.retry_count";

/**
 * The primary server name of the matched virtual host. This should be obtained via configuration.
 If no such configuration can be obtained, this attribute MUST NOT be set ( {@code net.host.name}
 should be used instead).
 *
 * <p>Notes:
  <ul> <li>{@code http.url} is usually not readily available on the server side but would have to be
 assembled in a cumbersome and sometimes lossy process from other information (see e.g.
 open-telemetry/opentelemetry-python/pull/148). It is thus preferred to supply the raw data that is
 available.</li> </ul>
 */
static constexpr const char *HTTP_SERVER_NAME = "http.server_name";

/**
 * The matched route (path template).
 */
static constexpr const char *HTTP_ROUTE = "http.route";

/**
 * The IP address of the original client behind all proxies, if known (e.g. from <a
href="https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Forwarded-For">X-Forwarded-For</a>).
 *
 * <p>Notes:
  <ul> <li>This is not necessarily the same as {@code net.peer.ip}, which would
identify the network-level peer, which may be a proxy.</li><li>This attribute should be set when a
source of information different from the one used for {@code net.peer.ip}, is available even if that
other source just confirms the same value as {@code net.peer.ip}. Rationale: For {@code
net.peer.ip}, one typically does not know if it comes from a proxy, reverse proxy, or the actual
client. Setting
{@code http.client_ip} when it's the same as {@code net.peer.ip} means that
one is at least somewhat confident that the address is not that of
the closest proxy.</li> </ul>
 */
static constexpr const char *HTTP_CLIENT_IP = "http.client_ip";

/**
 * The keys in the {@code RequestItems} object field.
 */
static constexpr const char *AWS_DYNAMODB_TABLE_NAMES = "aws.dynamodb.table_names";

/**
 * The JSON-serialized value of each item in the {@code ConsumedCapacity} response field.
 */
static constexpr const char *AWS_DYNAMODB_CONSUMED_CAPACITY = "aws.dynamodb.consumed_capacity";

/**
 * The JSON-serialized value of the {@code ItemCollectionMetrics} response field.
 */
static constexpr const char *AWS_DYNAMODB_ITEM_COLLECTION_METRICS =
    "aws.dynamodb.item_collection_metrics";

/**
 * The value of the {@code ProvisionedThroughput.ReadCapacityUnits} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_PROVISIONED_READ_CAPACITY =
    "aws.dynamodb.provisioned_read_capacity";

/**
 * The value of the {@code ProvisionedThroughput.WriteCapacityUnits} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_PROVISIONED_WRITE_CAPACITY =
    "aws.dynamodb.provisioned_write_capacity";

/**
 * The value of the {@code ConsistentRead} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_CONSISTENT_READ = "aws.dynamodb.consistent_read";

/**
 * The value of the {@code ProjectionExpression} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_PROJECTION = "aws.dynamodb.projection";

/**
 * The value of the {@code Limit} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_LIMIT = "aws.dynamodb.limit";

/**
 * The value of the {@code AttributesToGet} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_ATTRIBUTES_TO_GET = "aws.dynamodb.attributes_to_get";

/**
 * The value of the {@code IndexName} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_INDEX_NAME = "aws.dynamodb.index_name";

/**
 * The value of the {@code Select} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_SELECT = "aws.dynamodb.select";

/**
 * The JSON-serialized value of each item of the {@code GlobalSecondaryIndexes} request field
 */
static constexpr const char *AWS_DYNAMODB_GLOBAL_SECONDARY_INDEXES =
    "aws.dynamodb.global_secondary_indexes";

/**
 * The JSON-serialized value of each item of the {@code LocalSecondaryIndexes} request field.
 */
static constexpr const char *AWS_DYNAMODB_LOCAL_SECONDARY_INDEXES =
    "aws.dynamodb.local_secondary_indexes";

/**
 * The value of the {@code ExclusiveStartTableName} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_EXCLUSIVE_START_TABLE =
    "aws.dynamodb.exclusive_start_table";

/**
 * The the number of items in the {@code TableNames} response parameter.
 */
static constexpr const char *AWS_DYNAMODB_TABLE_COUNT = "aws.dynamodb.table_count";

/**
 * The value of the {@code ScanIndexForward} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_SCAN_FORWARD = "aws.dynamodb.scan_forward";

/**
 * The value of the {@code Segment} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_SEGMENT = "aws.dynamodb.segment";

/**
 * The value of the {@code TotalSegments} request parameter.
 */
static constexpr const char *AWS_DYNAMODB_TOTAL_SEGMENTS = "aws.dynamodb.total_segments";

/**
 * The value of the {@code Count} response parameter.
 */
static constexpr const char *AWS_DYNAMODB_COUNT = "aws.dynamodb.count";

/**
 * The value of the {@code ScannedCount} response parameter.
 */
static constexpr const char *AWS_DYNAMODB_SCANNED_COUNT = "aws.dynamodb.scanned_count";

/**
 * The JSON-serialized value of each item in the {@code AttributeDefinitions} request field.
 */
static constexpr const char *AWS_DYNAMODB_ATTRIBUTE_DEFINITIONS =
    "aws.dynamodb.attribute_definitions";

/**
 * The JSON-serialized value of each item in the the {@code GlobalSecondaryIndexUpdates} request
 * field.
 */
static constexpr const char *AWS_DYNAMODB_GLOBAL_SECONDARY_INDEX_UPDATES =
    "aws.dynamodb.global_secondary_index_updates";

/**
 * A string identifying the messaging system.
 */
static constexpr const char *MESSAGING_SYSTEM = "messaging.system";

/**
 * The message destination name. This might be equal to the span name but is required nevertheless.
 */
static constexpr const char *MESSAGING_DESTINATION = "messaging.destination";

/**
 * The kind of message destination
 */
static constexpr const char *MESSAGING_DESTINATION_KIND = "messaging.destination_kind";

/**
 * A boolean that is true if the message destination is temporary.
 */
static constexpr const char *MESSAGING_TEMP_DESTINATION = "messaging.temp_destination";

/**
 * The name of the transport protocol.
 */
static constexpr const char *MESSAGING_PROTOCOL = "messaging.protocol";

/**
 * The version of the transport protocol.
 */
static constexpr const char *MESSAGING_PROTOCOL_VERSION = "messaging.protocol_version";

/**
 * Connection string.
 */
static constexpr const char *MESSAGING_URL = "messaging.url";

/**
 * A value used by the messaging system as an identifier for the message, represented as a string.
 */
static constexpr const char *MESSAGING_MESSAGE_ID = "messaging.message_id";

/**
 * The <a href="#conversations">conversation ID</a> identifying the conversation to which the
 * message belongs, represented as a string. Sometimes called &quot;Correlation ID&quot;.
 */
static constexpr const char *MESSAGING_CONVERSATION_ID = "messaging.conversation_id";

/**
 * The (uncompressed) size of the message payload in bytes. Also use this attribute if it is unknown
 * whether the compressed or uncompressed payload size is reported.
 */
static constexpr const char *MESSAGING_MESSAGE_PAYLOAD_SIZE_BYTES =
    "messaging.message_payload_size_bytes";

/**
 * The compressed size of the message payload in bytes.
 */
static constexpr const char *MESSAGING_MESSAGE_PAYLOAD_COMPRESSED_SIZE_BYTES =
    "messaging.message_payload_compressed_size_bytes";

/**
 * A string identifying the kind of message consumption as defined in the <a
 * href="#operation-names">Operation names</a> section above. If the operation is &quot;send&quot;,
 * this attribute MUST NOT be set, since the operation can be inferred from the span kind in that
 * case.
 */
static constexpr const char *MESSAGING_OPERATION = "messaging.operation";

/**
 * The identifier for the consumer receiving a message. For Kafka, set it to {@code
 * {messaging.kafka.consumer_group} - {messaging.kafka.client_id}}, if both are present, or only
 * {@code messaging.kafka.consumer_group}. For brokers, such as RabbitMQ and Artemis, set it to the
 * {@code client_id} of the client consuming the message.
 */
static constexpr const char *MESSAGING_CONSUMER_ID = "messaging.consumer_id";

/**
 * RabbitMQ message routing key.
 */
static constexpr const char *MESSAGING_RABBITMQ_ROUTING_KEY = "messaging.rabbitmq.routing_key";

/**
 * Message keys in Kafka are used for grouping alike messages to ensure they're processed on the
 same partition. They differ from {@code messaging.message_id} in that they're not unique. If the
 key is {@code null}, the attribute MUST NOT be set.
 *
 * <p>Notes:
  <ul> <li>If the key type is not string, it's string representation has to be supplied for the
 attribute. If the key has no unambiguous, canonical string form, don't include its value.</li>
 </ul>
 */
static constexpr const char *MESSAGING_KAFKA_MESSAGE_KEY = "messaging.kafka.message_key";

/**
 * Name of the Kafka Consumer Group that is handling the message. Only applies to consumers, not
 * producers.
 */
static constexpr const char *MESSAGING_KAFKA_CONSUMER_GROUP = "messaging.kafka.consumer_group";

/**
 * Client Id for the Consumer or Producer that is handling the message.
 */
static constexpr const char *MESSAGING_KAFKA_CLIENT_ID = "messaging.kafka.client_id";

/**
 * Partition the message is sent to.
 */
static constexpr const char *MESSAGING_KAFKA_PARTITION = "messaging.kafka.partition";

/**
 * A boolean that is true if the message is a tombstone.
 */
static constexpr const char *MESSAGING_KAFKA_TOMBSTONE = "messaging.kafka.tombstone";

/**
 * Namespace of RocketMQ resources, resources in different namespaces are individual.
 */
static constexpr const char *MESSAGING_ROCKETMQ_NAMESPACE = "messaging.rocketmq.namespace";

/**
 * Name of the RocketMQ producer/consumer group that is handling the message. The client type is
 * identified by the SpanKind.
 */
static constexpr const char *MESSAGING_ROCKETMQ_CLIENT_GROUP = "messaging.rocketmq.client_group";

/**
 * The unique identifier for each client.
 */
static constexpr const char *MESSAGING_ROCKETMQ_CLIENT_ID = "messaging.rocketmq.client_id";

/**
 * Type of message.
 */
static constexpr const char *MESSAGING_ROCKETMQ_MESSAGE_TYPE = "messaging.rocketmq.message_type";

/**
 * The secondary classifier of message besides topic.
 */
static constexpr const char *MESSAGING_ROCKETMQ_MESSAGE_TAG = "messaging.rocketmq.message_tag";

/**
 * Key(s) of message, another way to mark message besides message id.
 */
static constexpr const char *MESSAGING_ROCKETMQ_MESSAGE_KEYS = "messaging.rocketmq.message_keys";

/**
 * Model of message consumption. This only applies to consumer spans.
 */
static constexpr const char *MESSAGING_ROCKETMQ_CONSUMPTION_MODEL =
    "messaging.rocketmq.consumption_model";

/**
 * A string identifying the remoting system. See below for a list of well-known identifiers.
 */
static constexpr const char *RPC_SYSTEM = "rpc.system";

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
static constexpr const char *RPC_SERVICE = "rpc.service";

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
static constexpr const char *RPC_METHOD = "rpc.method";

/**
 * The <a href="https://github.com/grpc/grpc/blob/v1.33.2/doc/statuscodes.md">numeric status
 * code</a> of the gRPC request.
 */
static constexpr const char *RPC_GRPC_STATUS_CODE = "rpc.grpc.status_code";

/**
 * Protocol version as in {@code jsonrpc} property of request/response. Since JSON-RPC 1.0 does not
 * specify this, the value can be omitted.
 */
static constexpr const char *RPC_JSONRPC_VERSION = "rpc.jsonrpc.version";

/**
 * {@code id} property of request or response. Since protocol allows id to be int, string, {@code
 * null} or missing (for notifications), value is expected to be cast to string for simplicity. Use
 * empty string in case of {@code null} value. Omit entirely if this is a notification.
 */
static constexpr const char *RPC_JSONRPC_REQUEST_ID = "rpc.jsonrpc.request_id";

/**
 * {@code error.code} property of response if it is an error response.
 */
static constexpr const char *RPC_JSONRPC_ERROR_CODE = "rpc.jsonrpc.error_code";

/**
 * {@code error.message} property of response if it is an error response.
 */
static constexpr const char *RPC_JSONRPC_ERROR_MESSAGE = "rpc.jsonrpc.error_message";

/**
 * Whether this is a received or sent message.
 */
static constexpr const char *MESSAGE_TYPE = "message.type";

/**
 * MUST be calculated as two different counters starting from {@code 1} one for sent messages and
 one for received message.
 *
 * <p>Notes:
  <ul> <li>This way we guarantee that the values will be consistent between different
 implementations.</li> </ul>
 */
static constexpr const char *MESSAGE_ID = "message.id";

/**
 * Compressed size of the message in bytes.
 */
static constexpr const char *MESSAGE_COMPRESSED_SIZE = "message.compressed_size";

/**
 * Uncompressed size of the message in bytes.
 */
static constexpr const char *MESSAGE_UNCOMPRESSED_SIZE = "message.uncompressed_size";

// Enum definitions
namespace OpentracingRefTypeValues
{
/** The parent Span depends on the child Span in some capacity. */
static constexpr const char *CHILD_OF = "child_of";
/** The parent Span does not depend in any way on the result of the child Span. */
static constexpr const char *FOLLOWS_FROM = "follows_from";
}  // namespace OpentracingRefTypeValues

namespace DbSystemValues
{
/** Some other SQL database. Fallback only. See notes. */
static constexpr const char *OTHER_SQL = "other_sql";
/** Microsoft SQL Server. */
static constexpr const char *MSSQL = "mssql";
/** MySQL. */
static constexpr const char *MYSQL = "mysql";
/** Oracle Database. */
static constexpr const char *ORACLE = "oracle";
/** IBM Db2. */
static constexpr const char *DB2 = "db2";
/** PostgreSQL. */
static constexpr const char *POSTGRESQL = "postgresql";
/** Amazon Redshift. */
static constexpr const char *REDSHIFT = "redshift";
/** Apache Hive. */
static constexpr const char *HIVE = "hive";
/** Cloudscape. */
static constexpr const char *CLOUDSCAPE = "cloudscape";
/** HyperSQL DataBase. */
static constexpr const char *HSQLDB = "hsqldb";
/** Progress Database. */
static constexpr const char *PROGRESS = "progress";
/** SAP MaxDB. */
static constexpr const char *MAXDB = "maxdb";
/** SAP HANA. */
static constexpr const char *HANADB = "hanadb";
/** Ingres. */
static constexpr const char *INGRES = "ingres";
/** FirstSQL. */
static constexpr const char *FIRSTSQL = "firstsql";
/** EnterpriseDB. */
static constexpr const char *EDB = "edb";
/** InterSystems Caché. */
static constexpr const char *CACHE = "cache";
/** Adabas (Adaptable Database System). */
static constexpr const char *ADABAS = "adabas";
/** Firebird. */
static constexpr const char *FIREBIRD = "firebird";
/** Apache Derby. */
static constexpr const char *DERBY = "derby";
/** FileMaker. */
static constexpr const char *FILEMAKER = "filemaker";
/** Informix. */
static constexpr const char *INFORMIX = "informix";
/** InstantDB. */
static constexpr const char *INSTANTDB = "instantdb";
/** InterBase. */
static constexpr const char *INTERBASE = "interbase";
/** MariaDB. */
static constexpr const char *MARIADB = "mariadb";
/** Netezza. */
static constexpr const char *NETEZZA = "netezza";
/** Pervasive PSQL. */
static constexpr const char *PERVASIVE = "pervasive";
/** PointBase. */
static constexpr const char *POINTBASE = "pointbase";
/** SQLite. */
static constexpr const char *SQLITE = "sqlite";
/** Sybase. */
static constexpr const char *SYBASE = "sybase";
/** Teradata. */
static constexpr const char *TERADATA = "teradata";
/** Vertica. */
static constexpr const char *VERTICA = "vertica";
/** H2. */
static constexpr const char *H2 = "h2";
/** ColdFusion IMQ. */
static constexpr const char *COLDFUSION = "coldfusion";
/** Apache Cassandra. */
static constexpr const char *CASSANDRA = "cassandra";
/** Apache HBase. */
static constexpr const char *HBASE = "hbase";
/** MongoDB. */
static constexpr const char *MONGODB = "mongodb";
/** Redis. */
static constexpr const char *REDIS = "redis";
/** Couchbase. */
static constexpr const char *COUCHBASE = "couchbase";
/** CouchDB. */
static constexpr const char *COUCHDB = "couchdb";
/** Microsoft Azure Cosmos DB. */
static constexpr const char *COSMOSDB = "cosmosdb";
/** Amazon DynamoDB. */
static constexpr const char *DYNAMODB = "dynamodb";
/** Neo4j. */
static constexpr const char *NEO4J = "neo4j";
/** Apache Geode. */
static constexpr const char *GEODE = "geode";
/** Elasticsearch. */
static constexpr const char *ELASTICSEARCH = "elasticsearch";
/** Memcached. */
static constexpr const char *MEMCACHED = "memcached";
/** CockroachDB. */
static constexpr const char *COCKROACHDB = "cockroachdb";
}  // namespace DbSystemValues

namespace DbCassandraConsistencyLevelValues
{
/** all. */
static constexpr const char *ALL = "all";
/** each_quorum. */
static constexpr const char *EACH_QUORUM = "each_quorum";
/** quorum. */
static constexpr const char *QUORUM = "quorum";
/** local_quorum. */
static constexpr const char *LOCAL_QUORUM = "local_quorum";
/** one. */
static constexpr const char *ONE = "one";
/** two. */
static constexpr const char *TWO = "two";
/** three. */
static constexpr const char *THREE = "three";
/** local_one. */
static constexpr const char *LOCAL_ONE = "local_one";
/** any. */
static constexpr const char *ANY = "any";
/** serial. */
static constexpr const char *SERIAL = "serial";
/** local_serial. */
static constexpr const char *LOCAL_SERIAL = "local_serial";
}  // namespace DbCassandraConsistencyLevelValues

namespace FaasTriggerValues
{
/** A response to some data source operation such as a database or filesystem read/write. */
static constexpr const char *DATASOURCE = "datasource";
/** To provide an answer to an inbound HTTP request. */
static constexpr const char *HTTP = "http";
/** A function is set to be executed when messages are sent to a messaging system. */
static constexpr const char *PUBSUB = "pubsub";
/** A function is scheduled to be executed regularly. */
static constexpr const char *TIMER = "timer";
/** If none of the others apply. */
static constexpr const char *OTHER = "other";
}  // namespace FaasTriggerValues

namespace FaasDocumentOperationValues
{
/** When a new object is created. */
static constexpr const char *INSERT = "insert";
/** When an object is modified. */
static constexpr const char *EDIT = "edit";
/** When an object is deleted. */
static constexpr const char *DELETE = "delete";
}  // namespace FaasDocumentOperationValues

namespace FaasInvokedProviderValues
{
/** Alibaba Cloud. */
static constexpr const char *ALIBABA_CLOUD = "alibaba_cloud";
/** Amazon Web Services. */
static constexpr const char *AWS = "aws";
/** Microsoft Azure. */
static constexpr const char *AZURE = "azure";
/** Google Cloud Platform. */
static constexpr const char *GCP = "gcp";
/** Tencent Cloud. */
static constexpr const char *TENCENT_CLOUD = "tencent_cloud";
}  // namespace FaasInvokedProviderValues

namespace NetTransportValues
{
/** ip_tcp. */
static constexpr const char *IP_TCP = "ip_tcp";
/** ip_udp. */
static constexpr const char *IP_UDP = "ip_udp";
/** Another IP-based protocol. */
static constexpr const char *IP = "ip";
/** Unix Domain socket. See below. */
static constexpr const char *UNIX = "unix";
/** Named or anonymous pipe. See note below. */
static constexpr const char *PIPE = "pipe";
/** In-process communication. */
static constexpr const char *INPROC = "inproc";
/** Something else (non IP-based). */
static constexpr const char *OTHER = "other";
}  // namespace NetTransportValues

namespace NetHostConnectionTypeValues
{
/** wifi. */
static constexpr const char *WIFI = "wifi";
/** wired. */
static constexpr const char *WIRED = "wired";
/** cell. */
static constexpr const char *CELL = "cell";
/** unavailable. */
static constexpr const char *UNAVAILABLE = "unavailable";
/** unknown. */
static constexpr const char *UNKNOWN = "unknown";
}  // namespace NetHostConnectionTypeValues

namespace NetHostConnectionSubtypeValues
{
/** GPRS. */
static constexpr const char *GPRS = "gprs";
/** EDGE. */
static constexpr const char *EDGE = "edge";
/** UMTS. */
static constexpr const char *UMTS = "umts";
/** CDMA. */
static constexpr const char *CDMA = "cdma";
/** EVDO Rel. 0. */
static constexpr const char *EVDO_0 = "evdo_0";
/** EVDO Rev. A. */
static constexpr const char *EVDO_A = "evdo_a";
/** CDMA2000 1XRTT. */
static constexpr const char *CDMA2000_1XRTT = "cdma2000_1xrtt";
/** HSDPA. */
static constexpr const char *HSDPA = "hsdpa";
/** HSUPA. */
static constexpr const char *HSUPA = "hsupa";
/** HSPA. */
static constexpr const char *HSPA = "hspa";
/** IDEN. */
static constexpr const char *IDEN = "iden";
/** EVDO Rev. B. */
static constexpr const char *EVDO_B = "evdo_b";
/** LTE. */
static constexpr const char *LTE = "lte";
/** EHRPD. */
static constexpr const char *EHRPD = "ehrpd";
/** HSPAP. */
static constexpr const char *HSPAP = "hspap";
/** GSM. */
static constexpr const char *GSM = "gsm";
/** TD-SCDMA. */
static constexpr const char *TD_SCDMA = "td_scdma";
/** IWLAN. */
static constexpr const char *IWLAN = "iwlan";
/** 5G NR (New Radio). */
static constexpr const char *NR = "nr";
/** 5G NRNSA (New Radio Non-Standalone). */
static constexpr const char *NRNSA = "nrnsa";
/** LTE CA. */
static constexpr const char *LTE_CA = "lte_ca";
}  // namespace NetHostConnectionSubtypeValues

namespace HttpFlavorValues
{
/** HTTP/1.0. */
static constexpr const char *HTTP_1_0 = "1.0";
/** HTTP/1.1. */
static constexpr const char *HTTP_1_1 = "1.1";
/** HTTP/2. */
static constexpr const char *HTTP_2_0 = "2.0";
/** HTTP/3. */
static constexpr const char *HTTP_3_0 = "3.0";
/** SPDY protocol. */
static constexpr const char *SPDY = "SPDY";
/** QUIC protocol. */
static constexpr const char *QUIC = "QUIC";
}  // namespace HttpFlavorValues

namespace MessagingDestinationKindValues
{
/** A message sent to a queue. */
static constexpr const char *QUEUE = "queue";
/** A message sent to a topic. */
static constexpr const char *TOPIC = "topic";
}  // namespace MessagingDestinationKindValues

namespace MessagingOperationValues
{
/** receive. */
static constexpr const char *RECEIVE = "receive";
/** process. */
static constexpr const char *PROCESS = "process";
}  // namespace MessagingOperationValues

namespace MessagingRocketmqMessageTypeValues
{
/** Normal message. */
static constexpr const char *NORMAL = "normal";
/** FIFO message. */
static constexpr const char *FIFO = "fifo";
/** Delay message. */
static constexpr const char *DELAY = "delay";
/** Transaction message. */
static constexpr const char *TRANSACTION = "transaction";
}  // namespace MessagingRocketmqMessageTypeValues

namespace MessagingRocketmqConsumptionModelValues
{
/** Clustering consumption model. */
static constexpr const char *CLUSTERING = "clustering";
/** Broadcasting consumption model. */
static constexpr const char *BROADCASTING = "broadcasting";
}  // namespace MessagingRocketmqConsumptionModelValues

namespace RpcSystemValues
{
/** gRPC. */
static constexpr const char *GRPC = "grpc";
/** Java RMI. */
static constexpr const char *JAVA_RMI = "java_rmi";
/** .NET WCF. */
static constexpr const char *DOTNET_WCF = "dotnet_wcf";
/** Apache Dubbo. */
static constexpr const char *APACHE_DUBBO = "apache_dubbo";
}  // namespace RpcSystemValues

namespace RpcGrpcStatusCodeValues
{
/** OK. */
static constexpr const int OK = 0;
/** CANCELLED. */
static constexpr const int CANCELLED = 1;
/** UNKNOWN. */
static constexpr const int UNKNOWN = 2;
/** INVALID_ARGUMENT. */
static constexpr const int INVALID_ARGUMENT = 3;
/** DEADLINE_EXCEEDED. */
static constexpr const int DEADLINE_EXCEEDED = 4;
/** NOT_FOUND. */
static constexpr const int NOT_FOUND = 5;
/** ALREADY_EXISTS. */
static constexpr const int ALREADY_EXISTS = 6;
/** PERMISSION_DENIED. */
static constexpr const int PERMISSION_DENIED = 7;
/** RESOURCE_EXHAUSTED. */
static constexpr const int RESOURCE_EXHAUSTED = 8;
/** FAILED_PRECONDITION. */
static constexpr const int FAILED_PRECONDITION = 9;
/** ABORTED. */
static constexpr const int ABORTED = 10;
/** OUT_OF_RANGE. */
static constexpr const int OUT_OF_RANGE = 11;
/** UNIMPLEMENTED. */
static constexpr const int UNIMPLEMENTED = 12;
/** INTERNAL. */
static constexpr const int INTERNAL = 13;
/** UNAVAILABLE. */
static constexpr const int UNAVAILABLE = 14;
/** DATA_LOSS. */
static constexpr const int DATA_LOSS = 15;
/** UNAUTHENTICATED. */
static constexpr const int UNAUTHENTICATED = 16;
}  // namespace RpcGrpcStatusCodeValues

namespace MessageTypeValues
{
/** sent. */
static constexpr const char *SENT = "SENT";
/** received. */
static constexpr const char *RECEIVED = "RECEIVED";
}  // namespace MessageTypeValues

}  // namespace SemanticConventions
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
