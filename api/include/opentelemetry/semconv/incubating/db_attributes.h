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
namespace db
{

/**
 * The consistency level of the query. Based on consistency values from <a
 * href="https://docs.datastax.com/en/cassandra-oss/3.0/cassandra/dml/dmlConfigConsistency.html">CQL</a>.
 */
static constexpr const char *kDbCassandraConsistencyLevel = "db.cassandra.consistency_level";

/**
 * The data center of the coordinating node for a query.
 */
static constexpr const char *kDbCassandraCoordinatorDc = "db.cassandra.coordinator.dc";

/**
 * The ID of the coordinating node for a query.
 */
static constexpr const char *kDbCassandraCoordinatorId = "db.cassandra.coordinator.id";

/**
 * Whether or not the query is idempotent.
 */
static constexpr const char *kDbCassandraIdempotence = "db.cassandra.idempotence";

/**
 * The fetch size used for paging, i.e. how many rows will be returned at once.
 */
static constexpr const char *kDbCassandraPageSize = "db.cassandra.page_size";

/**
 * The number of times a query was speculatively executed. Not set or @code 0 @endcode if the query
 * was not executed speculatively.
 */
static constexpr const char *kDbCassandraSpeculativeExecutionCount =
    "db.cassandra.speculative_execution_count";

/**
 * Deprecated, use @code db.collection.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.collection.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbCassandraTable = "db.cassandra.table";

/**
 * The name of the connection pool; unique within the instrumented application. In case the
 * connection pool implementation doesn't provide a name, instrumentation SHOULD use a combination
 * of parameters that would make the name unique, for example, combining attributes @code
 * server.address @endcode, @code server.port @endcode, and @code db.namespace @endcode, formatted
 * as @code server.address:server.port/db.namespace @endcode. Instrumentations that generate
 * connection pool name following different patterns SHOULD document it.
 */
static constexpr const char *kDbClientConnectionPoolName = "db.client.connection.pool.name";

/**
 * The state of a connection in the pool
 */
static constexpr const char *kDbClientConnectionState = "db.client.connection.state";

/**
 * Deprecated, use @code db.client.connection.pool.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.pool.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbClientConnectionsPoolName = "db.client.connections.pool.name";

/**
 * Deprecated, use @code db.client.connection.state @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.client.connection.state @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbClientConnectionsState = "db.client.connections.state";

/**
 * The name of a collection (table, container) within the database.
 * <p>
 * It is RECOMMENDED to capture the value as provided by the application without attempting to do
 * any case normalization. <p> The collection name SHOULD NOT be extracted from @code db.query.text
 * @endcode, unless the query format is known to only ever have a single collection name present.
 * <p>
 * For batch operations, if the individual operations are known to have the same collection name
 * then that collection name SHOULD be used.
 * <p>
 * This attribute has stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbCollectionName = "db.collection.name";

/**
 * Deprecated, use @code server.address @endcode, @code server.port @endcode attributes instead.
 * <p>
 * @deprecated
 * Replaced by @code server.address @endcode and @code server.port @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbConnectionString = "db.connection_string";

/**
 * Unique Cosmos client instance id.
 */
static constexpr const char *kDbCosmosdbClientId = "db.cosmosdb.client_id";

/**
 * Cosmos client connection mode.
 */
static constexpr const char *kDbCosmosdbConnectionMode = "db.cosmosdb.connection_mode";

/**
 * Account or request <a
 * href="https://learn.microsoft.com/azure/cosmos-db/consistency-levels">consistency level</a>.
 */
static constexpr const char *kDbCosmosdbConsistencyLevel = "db.cosmosdb.consistency_level";

/**
 * Deprecated, use @code db.collection.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.collection.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbCosmosdbContainer = "db.cosmosdb.container";

/**
 * Deprecated, no replacement at this time.
 * <p>
 * @deprecated
 * No replacement at this time.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbCosmosdbOperationType = "db.cosmosdb.operation_type";

/**
 * List of regions contacted during operation in the order that they were contacted. If there is
 * more than one region listed, it indicates that the operation was performed on multiple regions
 * i.e. cross-regional call. <p> Region name matches the format of @code displayName @endcode in <a
 * href="https://learn.microsoft.com/rest/api/subscription/subscriptions/list-locations?view=rest-subscription-2021-10-01&tabs=HTTP#location">Azure
 * Location API</a>
 */
static constexpr const char *kDbCosmosdbRegionsContacted = "db.cosmosdb.regions_contacted";

/**
 * Request units consumed for the operation.
 */
static constexpr const char *kDbCosmosdbRequestCharge = "db.cosmosdb.request_charge";

/**
 * Request payload size in bytes.
 */
static constexpr const char *kDbCosmosdbRequestContentLength = "db.cosmosdb.request_content_length";

/**
 * Deprecated, use @code db.response.status_code @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.response.status_code @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbCosmosdbStatusCode = "db.cosmosdb.status_code";

/**
 * Cosmos DB sub status code.
 */
static constexpr const char *kDbCosmosdbSubStatusCode = "db.cosmosdb.sub_status_code";

/**
 * Deprecated, use @code db.namespace @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.namespace @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbElasticsearchClusterName = "db.elasticsearch.cluster.name";

/**
 * Represents the human-readable identifier of the node/instance to which a request was routed.
 */
static constexpr const char *kDbElasticsearchNodeName = "db.elasticsearch.node.name";

/**
 * A dynamic value in the url path.
 * <p>
 * Many Elasticsearch url paths allow dynamic values. These SHOULD be recorded in span attributes in
 * the format @code db.elasticsearch.path_parts.<key> @endcode, where @code <key> @endcode is the
 * url path part name. The implementation SHOULD reference the <a
 * href="https://raw.githubusercontent.com/elastic/elasticsearch-specification/main/output/schema/schema.json">elasticsearch
 * schema</a> in order to map the path part values to their names.
 */
static constexpr const char *kDbElasticsearchPathParts = "db.elasticsearch.path_parts";

/**
 * Deprecated, no general replacement at this time. For Elasticsearch, use @code
 * db.elasticsearch.node.name @endcode instead. <p>
 * @deprecated
 * Deprecated, no general replacement at this time. For Elasticsearch, use @code
 * db.elasticsearch.node.name @endcode instead.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbInstanceId = "db.instance.id";

/**
 * Removed, no replacement at this time.
 * <p>
 * @deprecated
 * Removed as not used.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbJdbcDriverClassname = "db.jdbc.driver_classname";

/**
 * Deprecated, use @code db.collection.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.collection.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbMongodbCollection = "db.mongodb.collection";

/**
 * Deprecated, SQL Server instance is now populated as a part of @code db.namespace @endcode
 * attribute. <p>
 * @deprecated
 * Deprecated, no replacement at this time.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbMssqlInstanceName = "db.mssql.instance_name";

/**
 * Deprecated, use @code db.namespace @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.namespace @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbName = "db.name";

/**
 * The name of the database, fully qualified within the server address and port.
 * <p>
 * If a database system has multiple namespace components, they SHOULD be concatenated (potentially
 * using database system specific conventions) from most general to most specific namespace
 * component, and more specific namespaces SHOULD NOT be captured without the more general
 * namespaces, to ensure that "startswith" queries for the more general namespaces will be valid.
 * Semantic conventions for individual database systems SHOULD document what @code db.namespace
 * @endcode means in the context of that system. It is RECOMMENDED to capture the value as provided
 * by the application without attempting to do any case normalization. This attribute has stability
 * level RELEASE CANDIDATE.
 */
static constexpr const char *kDbNamespace = "db.namespace";

/**
 * Deprecated, use @code db.operation.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.operation.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbOperation = "db.operation";

/**
 * The number of queries included in a batch operation.
 * <p>
 * Operations are only considered batches when they contain two or more operations, and so @code
 * db.operation.batch.size @endcode SHOULD never be @code 1 @endcode. This attribute has stability
 * level RELEASE CANDIDATE.
 */
static constexpr const char *kDbOperationBatchSize = "db.operation.batch.size";

/**
 * The name of the operation or command being executed.
 * <p>
 * It is RECOMMENDED to capture the value as provided by the application
 * without attempting to do any case normalization.
 * <p>
 * The operation name SHOULD NOT be extracted from @code db.query.text @endcode,
 * unless the query format is known to only ever have a single operation name present.
 * <p>
 * For batch operations, if the individual operations are known to have the same operation name
 * then that operation name SHOULD be used prepended by @code BATCH  @endcode,
 * otherwise @code db.operation.name @endcode SHOULD be @code BATCH @endcode or some other database
 * system specific term if more applicable.
 * <p>
 * This attribute has stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbOperationName = "db.operation.name";

/**
 * A database operation parameter, with @code <key> @endcode being the parameter name, and the
 * attribute value being a string representation of the parameter value. <p> If a parameter has no
 * name and instead is referenced only by index, then @code <key> @endcode SHOULD be the 0-based
 * index. If @code db.query.text @endcode is also captured, then @code db.operation.parameter.<key>
 * @endcode SHOULD match up with the parameterized placeholders present in @code db.query.text
 * @endcode. This attribute has stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbOperationParameter = "db.operation.parameter";

/**
 * A query parameter used in @code db.query.text @endcode, with @code <key> @endcode being the
 * parameter name, and the attribute value being a string representation of the parameter value. <p>
 * @deprecated
 * Replaced by @code db.operation.parameter @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbQueryParameter = "db.query.parameter";

/**
 * Low cardinality representation of a database query text.
 * <p>
 * @code db.query.summary @endcode provides static summary of the query text. It describes a class
 * of database queries and is useful as a grouping key, especially when analyzing telemetry for
 * database calls involving complex queries. Summary may be available to the instrumentation through
 * instrumentation hooks or other means. If it is not available, instrumentations that support query
 * parsing SHOULD generate a summary following <a
 * href="../../docs/database/database-spans.md#generating-a-summary-of-the-query-text">Generating
 * query summary</a> section. This attribute has stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbQuerySummary = "db.query.summary";

/**
 * The database query being executed.
 * <p>
 * For sanitization see <a
 * href="../../docs/database/database-spans.md#sanitization-of-dbquerytext">Sanitization of @code
 * db.query.text @endcode</a>. For batch operations, if the individual operations are known to have
 * the same query text then that query text SHOULD be used, otherwise all of the individual query
 * texts SHOULD be concatenated with separator @code ;  @endcode or some other database system
 * specific separator if more applicable. Even though parameterized query text can potentially have
 * sensitive data, by using a parameterized query the user is giving a strong signal that any
 * sensitive data will be passed as parameter values, and the benefit to observability of capturing
 * the static part of the query text by default outweighs the risk. This attribute has stability
 * level RELEASE CANDIDATE.
 */
static constexpr const char *kDbQueryText = "db.query.text";

/**
 * Deprecated, use @code db.namespace @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.namespace @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbRedisDatabaseIndex = "db.redis.database_index";

/**
 * Number of rows returned by the operation.
 */
static constexpr const char *kDbResponseReturnedRows = "db.response.returned_rows";

/**
 * Database response status code.
 * <p>
 * The status code returned by the database. Usually it represents an error code, but may also
 * represent partial success, warning, or differentiate between various types of successful
 * outcomes. Semantic conventions for individual database systems SHOULD document what @code
 * db.response.status_code @endcode means in the context of that system. This attribute has
 * stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbResponseStatusCode = "db.response.status_code";

/**
 * Deprecated, use @code db.collection.name @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code db.collection.name @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbSqlTable = "db.sql.table";

/**
 * The database statement being executed.
 * <p>
 * @deprecated
 * Replaced by @code db.query.text @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbStatement = "db.statement";

/**
 * The database management system (DBMS) product as identified by the client instrumentation.
 * <p>
 * The actual DBMS may differ from the one identified by the client. For example, when using
 * PostgreSQL client libraries to connect to a CockroachDB, the @code db.system @endcode is set to
 * @code postgresql @endcode based on the instrumentation's best knowledge. This attribute has
 * stability level RELEASE CANDIDATE.
 */
static constexpr const char *kDbSystem = "db.system";

/**
 * Deprecated, no replacement at this time.
 * <p>
 * @deprecated
 * No replacement at this time.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kDbUser = "db.user";

namespace DbCassandraConsistencyLevelValues
{
/**
 * none
 */
static constexpr const char *kAll = "all";

/**
 * none
 */
static constexpr const char *kEachQuorum = "each_quorum";

/**
 * none
 */
static constexpr const char *kQuorum = "quorum";

/**
 * none
 */
static constexpr const char *kLocalQuorum = "local_quorum";

/**
 * none
 */
static constexpr const char *kOne = "one";

/**
 * none
 */
static constexpr const char *kTwo = "two";

/**
 * none
 */
static constexpr const char *kThree = "three";

/**
 * none
 */
static constexpr const char *kLocalOne = "local_one";

/**
 * none
 */
static constexpr const char *kAny = "any";

/**
 * none
 */
static constexpr const char *kSerial = "serial";

/**
 * none
 */
static constexpr const char *kLocalSerial = "local_serial";

}  // namespace DbCassandraConsistencyLevelValues

namespace DbClientConnectionStateValues
{
/**
 * none
 */
static constexpr const char *kIdle = "idle";

/**
 * none
 */
static constexpr const char *kUsed = "used";

}  // namespace DbClientConnectionStateValues

namespace DbClientConnectionsStateValues
{
/**
 * none
 */
static constexpr const char *kIdle = "idle";

/**
 * none
 */
static constexpr const char *kUsed = "used";

}  // namespace DbClientConnectionsStateValues

namespace DbCosmosdbConnectionModeValues
{
/**
 * Gateway (HTTP) connection.
 */
static constexpr const char *kGateway = "gateway";

/**
 * Direct connection.
 */
static constexpr const char *kDirect = "direct";

}  // namespace DbCosmosdbConnectionModeValues

namespace DbCosmosdbConsistencyLevelValues
{
/**
 * none
 */
static constexpr const char *kStrong = "Strong";

/**
 * none
 */
static constexpr const char *kBoundedStaleness = "BoundedStaleness";

/**
 * none
 */
static constexpr const char *kSession = "Session";

/**
 * none
 */
static constexpr const char *kEventual = "Eventual";

/**
 * none
 */
static constexpr const char *kConsistentPrefix = "ConsistentPrefix";

}  // namespace DbCosmosdbConsistencyLevelValues

namespace DbCosmosdbOperationTypeValues
{
/**
 * none
 */
static constexpr const char *kBatch = "batch";

/**
 * none
 */
static constexpr const char *kCreate = "create";

/**
 * none
 */
static constexpr const char *kDelete = "delete";

/**
 * none
 */
static constexpr const char *kExecute = "execute";

/**
 * none
 */
static constexpr const char *kExecuteJavascript = "execute_javascript";

/**
 * none
 */
static constexpr const char *kInvalid = "invalid";

/**
 * none
 */
static constexpr const char *kHead = "head";

/**
 * none
 */
static constexpr const char *kHeadFeed = "head_feed";

/**
 * none
 */
static constexpr const char *kPatch = "patch";

/**
 * none
 */
static constexpr const char *kQuery = "query";

/**
 * none
 */
static constexpr const char *kQueryPlan = "query_plan";

/**
 * none
 */
static constexpr const char *kRead = "read";

/**
 * none
 */
static constexpr const char *kReadFeed = "read_feed";

/**
 * none
 */
static constexpr const char *kReplace = "replace";

/**
 * none
 */
static constexpr const char *kUpsert = "upsert";

}  // namespace DbCosmosdbOperationTypeValues

namespace DbSystemValues
{
/**
 * Some other SQL database. Fallback only. See notes.
 */
static constexpr const char *kOtherSql = "other_sql";

/**
 * Adabas (Adaptable Database System)
 */
static constexpr const char *kAdabas = "adabas";

/**
 * Deprecated, use @code intersystems_cache @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code intersystems_cache @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kCache = "cache";

/**
 * InterSystems Caché
 */
static constexpr const char *kIntersystemsCache = "intersystems_cache";

/**
 * Apache Cassandra
 */
static constexpr const char *kCassandra = "cassandra";

/**
 * ClickHouse
 */
static constexpr const char *kClickhouse = "clickhouse";

/**
 * Deprecated, use @code other_sql @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code other_sql @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kCloudscape = "cloudscape";

/**
 * CockroachDB
 */
static constexpr const char *kCockroachdb = "cockroachdb";

/**
 * Deprecated, no replacement at this time.
 * <p>
 * @deprecated
 * Removed.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kColdfusion = "coldfusion";

/**
 * Microsoft Azure Cosmos DB
 */
static constexpr const char *kCosmosdb = "cosmosdb";

/**
 * Couchbase
 */
static constexpr const char *kCouchbase = "couchbase";

/**
 * CouchDB
 */
static constexpr const char *kCouchdb = "couchdb";

/**
 * IBM Db2
 */
static constexpr const char *kDb2 = "db2";

/**
 * Apache Derby
 */
static constexpr const char *kDerby = "derby";

/**
 * Amazon DynamoDB
 */
static constexpr const char *kDynamodb = "dynamodb";

/**
 * EnterpriseDB
 */
static constexpr const char *kEdb = "edb";

/**
 * Elasticsearch
 */
static constexpr const char *kElasticsearch = "elasticsearch";

/**
 * FileMaker
 */
static constexpr const char *kFilemaker = "filemaker";

/**
 * Firebird
 */
static constexpr const char *kFirebird = "firebird";

/**
 * Deprecated, use @code other_sql @endcode instead.
 * <p>
 * @deprecated
 * Replaced by @code other_sql @endcode.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kFirstsql = "firstsql";

/**
 * Apache Geode
 */
static constexpr const char *kGeode = "geode";

/**
 * H2
 */
static constexpr const char *kH2 = "h2";

/**
 * SAP HANA
 */
static constexpr const char *kHanadb = "hanadb";

/**
 * Apache HBase
 */
static constexpr const char *kHbase = "hbase";

/**
 * Apache Hive
 */
static constexpr const char *kHive = "hive";

/**
 * HyperSQL DataBase
 */
static constexpr const char *kHsqldb = "hsqldb";

/**
 * InfluxDB
 */
static constexpr const char *kInfluxdb = "influxdb";

/**
 * Informix
 */
static constexpr const char *kInformix = "informix";

/**
 * Ingres
 */
static constexpr const char *kIngres = "ingres";

/**
 * InstantDB
 */
static constexpr const char *kInstantdb = "instantdb";

/**
 * InterBase
 */
static constexpr const char *kInterbase = "interbase";

/**
 * MariaDB (This value has stability level RELEASE CANDIDATE)
 */
static constexpr const char *kMariadb = "mariadb";

/**
 * SAP MaxDB
 */
static constexpr const char *kMaxdb = "maxdb";

/**
 * Memcached
 */
static constexpr const char *kMemcached = "memcached";

/**
 * MongoDB
 */
static constexpr const char *kMongodb = "mongodb";

/**
 * Microsoft SQL Server (This value has stability level RELEASE CANDIDATE)
 */
static constexpr const char *kMssql = "mssql";

/**
 * Deprecated, Microsoft SQL Server Compact is discontinued.
 * <p>
 * @deprecated
 * Removed, use @code other_sql @endcode instead.
 */
OPENTELEMETRY_DEPRECATED
static constexpr const char *kMssqlcompact = "mssqlcompact";

/**
 * MySQL (This value has stability level RELEASE CANDIDATE)
 */
static constexpr const char *kMysql = "mysql";

/**
 * Neo4j
 */
static constexpr const char *kNeo4j = "neo4j";

/**
 * Netezza
 */
static constexpr const char *kNetezza = "netezza";

/**
 * OpenSearch
 */
static constexpr const char *kOpensearch = "opensearch";

/**
 * Oracle Database
 */
static constexpr const char *kOracle = "oracle";

/**
 * Pervasive PSQL
 */
static constexpr const char *kPervasive = "pervasive";

/**
 * PointBase
 */
static constexpr const char *kPointbase = "pointbase";

/**
 * PostgreSQL (This value has stability level RELEASE CANDIDATE)
 */
static constexpr const char *kPostgresql = "postgresql";

/**
 * Progress Database
 */
static constexpr const char *kProgress = "progress";

/**
 * Redis
 */
static constexpr const char *kRedis = "redis";

/**
 * Amazon Redshift
 */
static constexpr const char *kRedshift = "redshift";

/**
 * Cloud Spanner
 */
static constexpr const char *kSpanner = "spanner";

/**
 * SQLite
 */
static constexpr const char *kSqlite = "sqlite";

/**
 * Sybase
 */
static constexpr const char *kSybase = "sybase";

/**
 * Teradata
 */
static constexpr const char *kTeradata = "teradata";

/**
 * Trino
 */
static constexpr const char *kTrino = "trino";

/**
 * Vertica
 */
static constexpr const char *kVertica = "vertica";

}  // namespace DbSystemValues

}  // namespace db
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
