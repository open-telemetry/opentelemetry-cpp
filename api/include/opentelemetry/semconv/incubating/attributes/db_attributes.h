

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
static const char *kDbCassandraConsistencyLevel = "db.cassandra.consistency_level";

/**
 * The data center of the coordinating node for a query.
 */
static const char *kDbCassandraCoordinatorDc = "db.cassandra.coordinator.dc";

/**
 * The ID of the coordinating node for a query.
 */
static const char *kDbCassandraCoordinatorId = "db.cassandra.coordinator.id";

/**
 * Whether or not the query is idempotent.
 */
static const char *kDbCassandraIdempotence = "db.cassandra.idempotence";

/**
 * The fetch size used for paging, i.e. how many rows will be returned at once.
 */
static const char *kDbCassandraPageSize = "db.cassandra.page_size";

/**
 * The number of times a query was speculatively executed. Not set or @code 0 @endcode if the query
 * was not executed speculatively.
 */
static const char *kDbCassandraSpeculativeExecutionCount =
    "db.cassandra.speculative_execution_count";

/**
 * @Deprecated: Replaced by @code db.collection.name @endcode.
 */
static const char *kDbCassandraTable = "db.cassandra.table";

/**
 * The name of the connection pool; unique within the instrumented application. In case the
 * connection pool implementation doesn't provide a name, instrumentation SHOULD use a combination
 * of parameters that would make the name unique, for example, combining attributes @code
 * server.address @endcode, @code server.port @endcode, and @code db.namespace @endcode, formatted
 * as @code server.address:server.port/db.namespace @endcode. Instrumentations that generate
 * connection pool name following different patterns SHOULD document it.
 */
static const char *kDbClientConnectionPoolName = "db.client.connection.pool.name";

/**
 * The state of a connection in the pool.
 */
static const char *kDbClientConnectionState = "db.client.connection.state";

/**
 * @Deprecated: Replaced by @code db.client.connection.pool.name @endcode.
 */
static const char *kDbClientConnectionsPoolName = "db.client.connections.pool.name";

/**
 * @Deprecated: Replaced by @code db.client.connection.state @endcode.
 */
static const char *kDbClientConnectionsState = "db.client.connections.state";

/**
 * The name of a collection (table, container) within the database.
 * Note: It is RECOMMENDED to capture the value as provided by the application without attempting to
 * do any case normalization. If the collection name is parsed from the query text, it SHOULD be the
 * first collection name found in the query and it SHOULD match the value provided in the query text
 * including any schema and database name prefix. For batch operations, if the individual operations
 * are known to have the same collection name then that collection name SHOULD be used, otherwise
 * @code db.collection.name @endcode SHOULD NOT be captured.
 */
static const char *kDbCollectionName = "db.collection.name";

/**
 * @Deprecated: "Replaced by @code server.address @endcode and @code server.port @endcode.".
 */
static const char *kDbConnectionString = "db.connection_string";

/**
 * Unique Cosmos client instance id.
 */
static const char *kDbCosmosdbClientId = "db.cosmosdb.client_id";

/**
 * Cosmos client connection mode.
 */
static const char *kDbCosmosdbConnectionMode = "db.cosmosdb.connection_mode";

/**
 * @Deprecated: Replaced by @code db.collection.name @endcode.
 */
static const char *kDbCosmosdbContainer = "db.cosmosdb.container";

/**
 * CosmosDB Operation Type.
 */
static const char *kDbCosmosdbOperationType = "db.cosmosdb.operation_type";

/**
 * RU consumed for that operation.
 */
static const char *kDbCosmosdbRequestCharge = "db.cosmosdb.request_charge";

/**
 * Request payload size in bytes.
 */
static const char *kDbCosmosdbRequestContentLength = "db.cosmosdb.request_content_length";

/**
 * Cosmos DB status code.
 */
static const char *kDbCosmosdbStatusCode = "db.cosmosdb.status_code";

/**
 * Cosmos DB sub status code.
 */
static const char *kDbCosmosdbSubStatusCode = "db.cosmosdb.sub_status_code";

/**
 * @Deprecated: Replaced by @code db.namespace @endcode.
 */
static const char *kDbElasticsearchClusterName = "db.elasticsearch.cluster.name";

/**
 * Represents the human-readable identifier of the node/instance to which a request was routed.
 */
static const char *kDbElasticsearchNodeName = "db.elasticsearch.node.name";

/**
 * A dynamic value in the url path.
 * Note: Many Elasticsearch url paths allow dynamic values. These SHOULD be recorded in span
 * attributes in the format @code db.elasticsearch.path_parts.<key> @endcode, where @code <key>
 * @endcode is the url path part name. The implementation SHOULD reference the <a
 * href="https://raw.githubusercontent.com/elastic/elasticsearch-specification/main/output/schema/schema.json">elasticsearch
 * schema</a> in order to map the path part values to their names.
 */
static const char *kDbElasticsearchPathParts = "db.elasticsearch.path_parts";

/**
 * @Deprecated: Deprecated, no general replacement at this time. For Elasticsearch, use @code
 * db.elasticsearch.node.name @endcode instead.
 */
static const char *kDbInstanceId = "db.instance.id";

/**
 * @Deprecated: Removed as not used.
 */
static const char *kDbJdbcDriverClassname = "db.jdbc.driver_classname";

/**
 * @Deprecated: Replaced by @code db.collection.name @endcode.
 */
static const char *kDbMongodbCollection = "db.mongodb.collection";

/**
 * @Deprecated: Deprecated, no replacement at this time.
 */
static const char *kDbMssqlInstanceName = "db.mssql.instance_name";

/**
 * @Deprecated: Replaced by @code db.namespace @endcode.
 */
static const char *kDbName = "db.name";

/**
 * The name of the database, fully qualified within the server address and port.
 * Note: If a database system has multiple namespace components, they SHOULD be concatenated
 * (potentially using database system specific conventions) from most general to most specific
 * namespace component, and more specific namespaces SHOULD NOT be captured without the more general
 * namespaces, to ensure that "startswith" queries for the more general namespaces will be valid.
 * Semantic conventions for individual database systems SHOULD document what @code db.namespace
 * @endcode means in the context of that system. It is RECOMMENDED to capture the value as provided
 * by the application without attempting to do any case normalization.
 */
static const char *kDbNamespace = "db.namespace";

/**
 * @Deprecated: Replaced by @code db.operation.name @endcode.
 */
static const char *kDbOperation = "db.operation";

/**
 * The number of queries included in a <a
 * href="/docs/database/database-spans.md#batch-operations">batch operation</a>. Note: Operations
 * are only considered batches when they contain two or more operations, and so @code
 * db.operation.batch.size @endcode SHOULD never be @code 1 @endcode.
 */
static const char *kDbOperationBatchSize = "db.operation.batch.size";

/**
 * The name of the operation or command being executed.
 * Note: It is RECOMMENDED to capture the value as provided by the application without attempting to
 * do any case normalization. If the operation name is parsed from the query text, it SHOULD be the
 * first operation name found in the query. For batch operations, if the individual operations are
 * known to have the same operation name then that operation name SHOULD be used prepended by @code
 * BATCH  @endcode, otherwise @code db.operation.name @endcode SHOULD be @code BATCH @endcode or
 * some other database system specific term if more applicable.
 */
static const char *kDbOperationName = "db.operation.name";

/**
 * A query parameter used in @code db.query.text @endcode, with @code <key> @endcode being the
 * parameter name, and the attribute value being a string representation of the parameter value.
 * Note: Query parameters should only be captured when @code db.query.text @endcode is parameterized
 * with placeholders. If a parameter has no name and instead is referenced only by index, then @code
 * <key> @endcode SHOULD be the 0-based index.
 */
static const char *kDbQueryParameter = "db.query.parameter";

/**
 * The database query being executed.
 * Note: For sanitization see <a
 * href="../../docs/database/database-spans.md#sanitization-of-dbquerytext">Sanitization of @code
 * db.query.text @endcode</a>. For batch operations, if the individual operations are known to have
 * the same query text then that query text SHOULD be used, otherwise all of the individual query
 * texts SHOULD be concatenated with separator @code ;  @endcode or some other database system
 * specific separator if more applicable. Even though parameterized query text can potentially have
 * sensitive data, by using a parameterized query the user is giving a strong signal that any
 * sensitive data will be passed as parameter values, and the benefit to observability of capturing
 * the static part of the query text by default outweighs the risk.
 */
static const char *kDbQueryText = "db.query.text";

/**
 * @Deprecated: Replaced by @code db.namespace @endcode.
 */
static const char *kDbRedisDatabaseIndex = "db.redis.database_index";

/**
 * @Deprecated: Replaced by @code db.collection.name @endcode.
 */
static const char *kDbSqlTable = "db.sql.table";

/**
 * @Deprecated: Replaced by @code db.query.text @endcode.
 */
static const char *kDbStatement = "db.statement";

/**
 * The database management system (DBMS) product as identified by the client instrumentation.
 * Note: The actual DBMS may differ from the one identified by the client. For example, when using
 * PostgreSQL client libraries to connect to a CockroachDB, the @code db.system @endcode is set to
 * @code postgresql @endcode based on the instrumentation's best knowledge.
 */
static const char *kDbSystem = "db.system";

/**
 * @Deprecated: No replacement at this time.
 */
static const char *kDbUser = "db.user";

// DEBUG: {"brief": "The consistency level of the query. Based on consistency values from
// [CQL](https://docs.datastax.com/en/cassandra-oss/3.0/cassandra/dml/dmlConfigConsistency.html).\n",
// "name": "db.cassandra.consistency_level", "requirement_level": "recommended", "root_namespace":
// "db", "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief":
// none, "deprecated": none, "id": "all", "note": none, "stability": "experimental", "value":
// "all"}, {"brief": none, "deprecated": none, "id": "each_quorum", "note": none, "stability":
// "experimental", "value": "each_quorum"}, {"brief": none, "deprecated": none, "id": "quorum",
// "note": none, "stability": "experimental", "value": "quorum"}, {"brief": none, "deprecated":
// none, "id": "local_quorum", "note": none, "stability": "experimental", "value": "local_quorum"},
// {"brief": none, "deprecated": none, "id": "one", "note": none, "stability": "experimental",
// "value": "one"}, {"brief": none, "deprecated": none, "id": "two", "note": none, "stability":
// "experimental", "value": "two"}, {"brief": none, "deprecated": none, "id": "three", "note": none,
// "stability": "experimental", "value": "three"}, {"brief": none, "deprecated": none, "id":
// "local_one", "note": none, "stability": "experimental", "value": "local_one"}, {"brief": none,
// "deprecated": none, "id": "any", "note": none, "stability": "experimental", "value": "any"},
// {"brief": none, "deprecated": none, "id": "serial", "note": none, "stability": "experimental",
// "value": "serial"}, {"brief": none, "deprecated": none, "id": "local_serial", "note": none,
// "stability": "experimental", "value": "local_serial"}]}}
namespace DbCassandraConsistencyLevelValues
{
/**
 * all.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "all", "note": none, "stability":
// "experimental", "value": "all"}
static constexpr const char *kAll = "all";
/**
 * each_quorum.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "each_quorum", "note": none, "stability":
// "experimental", "value": "each_quorum"}
static constexpr const char *kEachQuorum = "each_quorum";
/**
 * quorum.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "quorum", "note": none, "stability":
// "experimental", "value": "quorum"}
static constexpr const char *kQuorum = "quorum";
/**
 * local_quorum.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "local_quorum", "note": none, "stability":
// "experimental", "value": "local_quorum"}
static constexpr const char *kLocalQuorum = "local_quorum";
/**
 * one.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "one", "note": none, "stability":
// "experimental", "value": "one"}
static constexpr const char *kOne = "one";
/**
 * two.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "two", "note": none, "stability":
// "experimental", "value": "two"}
static constexpr const char *kTwo = "two";
/**
 * three.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "three", "note": none, "stability":
// "experimental", "value": "three"}
static constexpr const char *kThree = "three";
/**
 * local_one.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "local_one", "note": none, "stability":
// "experimental", "value": "local_one"}
static constexpr const char *kLocalOne = "local_one";
/**
 * any.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "any", "note": none, "stability":
// "experimental", "value": "any"}
static constexpr const char *kAny = "any";
/**
 * serial.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "serial", "note": none, "stability":
// "experimental", "value": "serial"}
static constexpr const char *kSerial = "serial";
/**
 * local_serial.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "local_serial", "note": none, "stability":
// "experimental", "value": "local_serial"}
static constexpr const char *kLocalSerial = "local_serial";
}  // namespace DbCassandraConsistencyLevelValues

// DEBUG: {"brief": "The state of a connection in the pool", "examples": ["idle"], "name":
// "db.client.connection.state", "requirement_level": "recommended", "root_namespace": "db",
// "stability": "experimental", "type": {"allow_custom_values": true, "members": [{"brief": none,
// "deprecated": none, "id": "idle", "note": none, "stability": "experimental", "value": "idle"},
// {"brief": none, "deprecated": none, "id": "used", "note": none, "stability": "experimental",
// "value": "used"}]}}
namespace DbClientConnectionStateValues
{
/**
 * idle.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
}  // namespace DbClientConnectionStateValues

// @deprecated(reason="The attribute db.client.connections.state is deprecated - Replaced by
// `db.client.connection.state`")  # type: ignore DEBUG: {"brief": "Deprecated, use
// `db.client.connection.state` instead.", "deprecated": "Replaced by
// `db.client.connection.state`.", "examples": ["idle"], "name": "db.client.connections.state",
// "requirement_level": "recommended", "root_namespace": "db", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "idle",
// "note": none, "stability": "experimental", "value": "idle"}, {"brief": none, "deprecated": none,
// "id": "used", "note": none, "stability": "experimental", "value": "used"}]}}
namespace DbClientConnectionsStateValues
{
/**
 * idle.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "idle", "note": none, "stability":
// "experimental", "value": "idle"}
static constexpr const char *kIdle = "idle";
/**
 * used.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "used", "note": none, "stability":
// "experimental", "value": "used"}
static constexpr const char *kUsed = "used";
}  // namespace DbClientConnectionsStateValues

// DEBUG: {"brief": "Cosmos client connection mode.", "name": "db.cosmosdb.connection_mode",
// "requirement_level": "recommended", "root_namespace": "db", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": "Gateway (HTTP) connections mode",
// "deprecated": none, "id": "gateway", "note": none, "stability": "experimental", "value":
// "gateway"}, {"brief": "Direct connection.", "deprecated": none, "id": "direct", "note": none,
// "stability": "experimental", "value": "direct"}]}}
namespace DbCosmosdbConnectionModeValues
{
/**
 * Gateway (HTTP) connections mode.
 */
// DEBUG: {"brief": "Gateway (HTTP) connections mode", "deprecated": none, "id": "gateway", "note":
// none, "stability": "experimental", "value": "gateway"}
static constexpr const char *kGateway = "gateway";
/**
 * Direct connection.
 */
// DEBUG: {"brief": "Direct connection.", "deprecated": none, "id": "direct", "note": none,
// "stability": "experimental", "value": "direct"}
static constexpr const char *kDirect = "direct";
}  // namespace DbCosmosdbConnectionModeValues

// DEBUG: {"brief": "CosmosDB Operation Type.", "name": "db.cosmosdb.operation_type",
// "requirement_level": "recommended", "root_namespace": "db", "stability": "experimental", "type":
// {"allow_custom_values": true, "members": [{"brief": none, "deprecated": none, "id": "invalid",
// "note": none, "stability": "experimental", "value": "Invalid"}, {"brief": none, "deprecated":
// none, "id": "create", "note": none, "stability": "experimental", "value": "Create"}, {"brief":
// none, "deprecated": none, "id": "patch", "note": none, "stability": "experimental", "value":
// "Patch"}, {"brief": none, "deprecated": none, "id": "read", "note": none, "stability":
// "experimental", "value": "Read"}, {"brief": none, "deprecated": none, "id": "read_feed", "note":
// none, "stability": "experimental", "value": "ReadFeed"}, {"brief": none, "deprecated": none,
// "id": "delete", "note": none, "stability": "experimental", "value": "Delete"}, {"brief": none,
// "deprecated": none, "id": "replace", "note": none, "stability": "experimental", "value":
// "Replace"}, {"brief": none, "deprecated": none, "id": "execute", "note": none, "stability":
// "experimental", "value": "Execute"}, {"brief": none, "deprecated": none, "id": "query", "note":
// none, "stability": "experimental", "value": "Query"}, {"brief": none, "deprecated": none, "id":
// "head", "note": none, "stability": "experimental", "value": "Head"}, {"brief": none,
// "deprecated": none, "id": "head_feed", "note": none, "stability": "experimental", "value":
// "HeadFeed"}, {"brief": none, "deprecated": none, "id": "upsert", "note": none, "stability":
// "experimental", "value": "Upsert"}, {"brief": none, "deprecated": none, "id": "batch", "note":
// none, "stability": "experimental", "value": "Batch"}, {"brief": none, "deprecated": none, "id":
// "query_plan", "note": none, "stability": "experimental", "value": "QueryPlan"}, {"brief": none,
// "deprecated": none, "id": "execute_javascript", "note": none, "stability": "experimental",
// "value": "ExecuteJavaScript"}]}}
namespace DbCosmosdbOperationTypeValues
{
/**
 * invalid.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "invalid", "note": none, "stability":
// "experimental", "value": "Invalid"}
static constexpr const char *kInvalid = "Invalid";
/**
 * create.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "create", "note": none, "stability":
// "experimental", "value": "Create"}
static constexpr const char *kCreate = "Create";
/**
 * patch.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "patch", "note": none, "stability":
// "experimental", "value": "Patch"}
static constexpr const char *kPatch = "Patch";
/**
 * read.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "read", "note": none, "stability":
// "experimental", "value": "Read"}
static constexpr const char *kRead = "Read";
/**
 * read_feed.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "read_feed", "note": none, "stability":
// "experimental", "value": "ReadFeed"}
static constexpr const char *kReadFeed = "ReadFeed";
/**
 * delete.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "delete", "note": none, "stability":
// "experimental", "value": "Delete"}
static constexpr const char *kDelete = "Delete";
/**
 * replace.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "replace", "note": none, "stability":
// "experimental", "value": "Replace"}
static constexpr const char *kReplace = "Replace";
/**
 * execute.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "execute", "note": none, "stability":
// "experimental", "value": "Execute"}
static constexpr const char *kExecute = "Execute";
/**
 * query.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "query", "note": none, "stability":
// "experimental", "value": "Query"}
static constexpr const char *kQuery = "Query";
/**
 * head.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "head", "note": none, "stability":
// "experimental", "value": "Head"}
static constexpr const char *kHead = "Head";
/**
 * head_feed.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "head_feed", "note": none, "stability":
// "experimental", "value": "HeadFeed"}
static constexpr const char *kHeadFeed = "HeadFeed";
/**
 * upsert.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "upsert", "note": none, "stability":
// "experimental", "value": "Upsert"}
static constexpr const char *kUpsert = "Upsert";
/**
 * batch.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "batch", "note": none, "stability":
// "experimental", "value": "Batch"}
static constexpr const char *kBatch = "Batch";
/**
 * query_plan.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "query_plan", "note": none, "stability":
// "experimental", "value": "QueryPlan"}
static constexpr const char *kQueryPlan = "QueryPlan";
/**
 * execute_javascript.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "execute_javascript", "note": none, "stability":
// "experimental", "value": "ExecuteJavaScript"}
static constexpr const char *kExecuteJavascript = "ExecuteJavaScript";
}  // namespace DbCosmosdbOperationTypeValues

// DEBUG: {"brief": "The database management system (DBMS) product as identified by the client
// instrumentation.", "name": "db.system", "note": "The actual DBMS may differ from the one
// identified by the client. For example, when using PostgreSQL client libraries to connect to a
// CockroachDB, the `db.system` is set to `postgresql` based on the instrumentation's best
// knowledge.\n", "requirement_level": "recommended", "root_namespace": "db", "stability":
// "experimental", "type": {"allow_custom_values": true, "members": [{"brief": "Some other SQL
// database. Fallback only. See notes.", "deprecated": none, "id": "other_sql", "note": none,
// "stability": "experimental", "value": "other_sql"}, {"brief": "Adabas (Adaptable Database
// System)", "deprecated": none, "id": "adabas", "note": none, "stability": "experimental", "value":
// "adabas"}, {"brief": "Deprecated, use `intersystems_cache` instead.", "deprecated": "Replaced by
// `intersystems_cache`.", "id": "cache", "note": none, "stability": "experimental", "value":
// "cache"}, {"brief": "InterSystems Caché", "deprecated": none, "id": "intersystems_cache", "note":
// none, "stability": "experimental", "value": "intersystems_cache"}, {"brief": "Apache Cassandra",
// "deprecated": none, "id": "cassandra", "note": none, "stability": "experimental", "value":
// "cassandra"}, {"brief": "ClickHouse", "deprecated": none, "id": "clickhouse", "note": none,
// "stability": "experimental", "value": "clickhouse"}, {"brief": "Deprecated, use `other_sql`
// instead.", "deprecated": "Replaced by `other_sql`.", "id": "cloudscape", "note": none,
// "stability": "experimental", "value": "cloudscape"}, {"brief": "CockroachDB", "deprecated": none,
// "id": "cockroachdb", "note": none, "stability": "experimental", "value": "cockroachdb"},
// {"brief": "Deprecated, no replacement at this time.", "deprecated": "Removed.", "id":
// "coldfusion", "note": none, "stability": "experimental", "value": "coldfusion"}, {"brief":
// "Microsoft Azure Cosmos DB", "deprecated": none, "id": "cosmosdb", "note": none, "stability":
// "experimental", "value": "cosmosdb"}, {"brief": "Couchbase", "deprecated": none, "id":
// "couchbase", "note": none, "stability": "experimental", "value": "couchbase"}, {"brief":
// "CouchDB", "deprecated": none, "id": "couchdb", "note": none, "stability": "experimental",
// "value": "couchdb"}, {"brief": "IBM Db2", "deprecated": none, "id": "db2", "note": none,
// "stability": "experimental", "value": "db2"}, {"brief": "Apache Derby", "deprecated": none, "id":
// "derby", "note": none, "stability": "experimental", "value": "derby"}, {"brief": "Amazon
// DynamoDB", "deprecated": none, "id": "dynamodb", "note": none, "stability": "experimental",
// "value": "dynamodb"}, {"brief": "EnterpriseDB", "deprecated": none, "id": "edb", "note": none,
// "stability": "experimental", "value": "edb"}, {"brief": "Elasticsearch", "deprecated": none,
// "id": "elasticsearch", "note": none, "stability": "experimental", "value": "elasticsearch"},
// {"brief": "FileMaker", "deprecated": none, "id": "filemaker", "note": none, "stability":
// "experimental", "value": "filemaker"}, {"brief": "Firebird", "deprecated": none, "id":
// "firebird", "note": none, "stability": "experimental", "value": "firebird"}, {"brief":
// "Deprecated, use `other_sql` instead.", "deprecated": "Replaced by `other_sql`.", "id":
// "firstsql", "note": none, "stability": "experimental", "value": "firstsql"}, {"brief": "Apache
// Geode", "deprecated": none, "id": "geode", "note": none, "stability": "experimental", "value":
// "geode"}, {"brief": "H2", "deprecated": none, "id": "h2", "note": none, "stability":
// "experimental", "value": "h2"}, {"brief": "SAP HANA", "deprecated": none, "id": "hanadb", "note":
// none, "stability": "experimental", "value": "hanadb"}, {"brief": "Apache HBase", "deprecated":
// none, "id": "hbase", "note": none, "stability": "experimental", "value": "hbase"}, {"brief":
// "Apache Hive", "deprecated": none, "id": "hive", "note": none, "stability": "experimental",
// "value": "hive"}, {"brief": "HyperSQL DataBase", "deprecated": none, "id": "hsqldb", "note":
// none, "stability": "experimental", "value": "hsqldb"}, {"brief": "InfluxDB", "deprecated": none,
// "id": "influxdb", "note": none, "stability": "experimental", "value": "influxdb"}, {"brief":
// "Informix", "deprecated": none, "id": "informix", "note": none, "stability": "experimental",
// "value": "informix"}, {"brief": "Ingres", "deprecated": none, "id": "ingres", "note": none,
// "stability": "experimental", "value": "ingres"}, {"brief": "InstantDB", "deprecated": none, "id":
// "instantdb", "note": none, "stability": "experimental", "value": "instantdb"}, {"brief":
// "InterBase", "deprecated": none, "id": "interbase", "note": none, "stability": "experimental",
// "value": "interbase"}, {"brief": "MariaDB", "deprecated": none, "id": "mariadb", "note": none,
// "stability": "experimental", "value": "mariadb"}, {"brief": "SAP MaxDB", "deprecated": none,
// "id": "maxdb", "note": none, "stability": "experimental", "value": "maxdb"}, {"brief":
// "Memcached", "deprecated": none, "id": "memcached", "note": none, "stability": "experimental",
// "value": "memcached"}, {"brief": "MongoDB", "deprecated": none, "id": "mongodb", "note": none,
// "stability": "experimental", "value": "mongodb"}, {"brief": "Microsoft SQL Server", "deprecated":
// none, "id": "mssql", "note": none, "stability": "experimental", "value": "mssql"}, {"brief":
// "Deprecated, Microsoft SQL Server Compact is discontinued.", "deprecated": "Removed, use
// `other_sql` instead.", "id": "mssqlcompact", "note": none, "stability": "experimental", "value":
// "mssqlcompact"}, {"brief": "MySQL", "deprecated": none, "id": "mysql", "note": none, "stability":
// "experimental", "value": "mysql"}, {"brief": "Neo4j", "deprecated": none, "id": "neo4j", "note":
// none, "stability": "experimental", "value": "neo4j"}, {"brief": "Netezza", "deprecated": none,
// "id": "netezza", "note": none, "stability": "experimental", "value": "netezza"}, {"brief":
// "OpenSearch", "deprecated": none, "id": "opensearch", "note": none, "stability": "experimental",
// "value": "opensearch"}, {"brief": "Oracle Database", "deprecated": none, "id": "oracle", "note":
// none, "stability": "experimental", "value": "oracle"}, {"brief": "Pervasive PSQL", "deprecated":
// none, "id": "pervasive", "note": none, "stability": "experimental", "value": "pervasive"},
// {"brief": "PointBase", "deprecated": none, "id": "pointbase", "note": none, "stability":
// "experimental", "value": "pointbase"}, {"brief": "PostgreSQL", "deprecated": none, "id":
// "postgresql", "note": none, "stability": "experimental", "value": "postgresql"}, {"brief":
// "Progress Database", "deprecated": none, "id": "progress", "note": none, "stability":
// "experimental", "value": "progress"}, {"brief": "Redis", "deprecated": none, "id": "redis",
// "note": none, "stability": "experimental", "value": "redis"}, {"brief": "Amazon Redshift",
// "deprecated": none, "id": "redshift", "note": none, "stability": "experimental", "value":
// "redshift"}, {"brief": "Cloud Spanner", "deprecated": none, "id": "spanner", "note": none,
// "stability": "experimental", "value": "spanner"}, {"brief": "SQLite", "deprecated": none, "id":
// "sqlite", "note": none, "stability": "experimental", "value": "sqlite"}, {"brief": "Sybase",
// "deprecated": none, "id": "sybase", "note": none, "stability": "experimental", "value":
// "sybase"}, {"brief": "Teradata", "deprecated": none, "id": "teradata", "note": none, "stability":
// "experimental", "value": "teradata"}, {"brief": "Trino", "deprecated": none, "id": "trino",
// "note": none, "stability": "experimental", "value": "trino"}, {"brief": "Vertica", "deprecated":
// none, "id": "vertica", "note": none, "stability": "experimental", "value": "vertica"}]}}
namespace DbSystemValues
{
/**
 * Some other SQL database. Fallback only. See notes.
 */
// DEBUG: {"brief": "Some other SQL database. Fallback only. See notes.", "deprecated": none, "id":
// "other_sql", "note": none, "stability": "experimental", "value": "other_sql"}
static constexpr const char *kOtherSql = "other_sql";
/**
 * Adabas (Adaptable Database System).
 */
// DEBUG: {"brief": "Adabas (Adaptable Database System)", "deprecated": none, "id": "adabas",
// "note": none, "stability": "experimental", "value": "adabas"}
static constexpr const char *kAdabas = "adabas";
/**
 * @Deprecated: Replaced by @code intersystems_cache @endcode.
 */
// DEBUG: {"brief": "Deprecated, use `intersystems_cache` instead.", "deprecated": "Replaced by
// `intersystems_cache`.", "id": "cache", "note": none, "stability": "experimental", "value":
// "cache"}
static constexpr const char *kCache = "cache";
/**
 * InterSystems Caché.
 */
// DEBUG: {"brief": "InterSystems Caché", "deprecated": none, "id": "intersystems_cache", "note":
// none, "stability": "experimental", "value": "intersystems_cache"}
static constexpr const char *kIntersystemsCache = "intersystems_cache";
/**
 * Apache Cassandra.
 */
// DEBUG: {"brief": "Apache Cassandra", "deprecated": none, "id": "cassandra", "note": none,
// "stability": "experimental", "value": "cassandra"}
static constexpr const char *kCassandra = "cassandra";
/**
 * ClickHouse.
 */
// DEBUG: {"brief": "ClickHouse", "deprecated": none, "id": "clickhouse", "note": none, "stability":
// "experimental", "value": "clickhouse"}
static constexpr const char *kClickhouse = "clickhouse";
/**
 * @Deprecated: Replaced by @code other_sql @endcode.
 */
// DEBUG: {"brief": "Deprecated, use `other_sql` instead.", "deprecated": "Replaced by
// `other_sql`.", "id": "cloudscape", "note": none, "stability": "experimental", "value":
// "cloudscape"}
static constexpr const char *kCloudscape = "cloudscape";
/**
 * CockroachDB.
 */
// DEBUG: {"brief": "CockroachDB", "deprecated": none, "id": "cockroachdb", "note": none,
// "stability": "experimental", "value": "cockroachdb"}
static constexpr const char *kCockroachdb = "cockroachdb";
/**
 * @Deprecated: Removed.
 */
// DEBUG: {"brief": "Deprecated, no replacement at this time.", "deprecated": "Removed.", "id":
// "coldfusion", "note": none, "stability": "experimental", "value": "coldfusion"}
static constexpr const char *kColdfusion = "coldfusion";
/**
 * Microsoft Azure Cosmos DB.
 */
// DEBUG: {"brief": "Microsoft Azure Cosmos DB", "deprecated": none, "id": "cosmosdb", "note": none,
// "stability": "experimental", "value": "cosmosdb"}
static constexpr const char *kCosmosdb = "cosmosdb";
/**
 * Couchbase.
 */
// DEBUG: {"brief": "Couchbase", "deprecated": none, "id": "couchbase", "note": none, "stability":
// "experimental", "value": "couchbase"}
static constexpr const char *kCouchbase = "couchbase";
/**
 * CouchDB.
 */
// DEBUG: {"brief": "CouchDB", "deprecated": none, "id": "couchdb", "note": none, "stability":
// "experimental", "value": "couchdb"}
static constexpr const char *kCouchdb = "couchdb";
/**
 * IBM Db2.
 */
// DEBUG: {"brief": "IBM Db2", "deprecated": none, "id": "db2", "note": none, "stability":
// "experimental", "value": "db2"}
static constexpr const char *kDb2 = "db2";
/**
 * Apache Derby.
 */
// DEBUG: {"brief": "Apache Derby", "deprecated": none, "id": "derby", "note": none, "stability":
// "experimental", "value": "derby"}
static constexpr const char *kDerby = "derby";
/**
 * Amazon DynamoDB.
 */
// DEBUG: {"brief": "Amazon DynamoDB", "deprecated": none, "id": "dynamodb", "note": none,
// "stability": "experimental", "value": "dynamodb"}
static constexpr const char *kDynamodb = "dynamodb";
/**
 * EnterpriseDB.
 */
// DEBUG: {"brief": "EnterpriseDB", "deprecated": none, "id": "edb", "note": none, "stability":
// "experimental", "value": "edb"}
static constexpr const char *kEdb = "edb";
/**
 * Elasticsearch.
 */
// DEBUG: {"brief": "Elasticsearch", "deprecated": none, "id": "elasticsearch", "note": none,
// "stability": "experimental", "value": "elasticsearch"}
static constexpr const char *kElasticsearch = "elasticsearch";
/**
 * FileMaker.
 */
// DEBUG: {"brief": "FileMaker", "deprecated": none, "id": "filemaker", "note": none, "stability":
// "experimental", "value": "filemaker"}
static constexpr const char *kFilemaker = "filemaker";
/**
 * Firebird.
 */
// DEBUG: {"brief": "Firebird", "deprecated": none, "id": "firebird", "note": none, "stability":
// "experimental", "value": "firebird"}
static constexpr const char *kFirebird = "firebird";
/**
 * @Deprecated: Replaced by @code other_sql @endcode.
 */
// DEBUG: {"brief": "Deprecated, use `other_sql` instead.", "deprecated": "Replaced by
// `other_sql`.", "id": "firstsql", "note": none, "stability": "experimental", "value": "firstsql"}
static constexpr const char *kFirstsql = "firstsql";
/**
 * Apache Geode.
 */
// DEBUG: {"brief": "Apache Geode", "deprecated": none, "id": "geode", "note": none, "stability":
// "experimental", "value": "geode"}
static constexpr const char *kGeode = "geode";
/**
 * H2.
 */
// DEBUG: {"brief": "H2", "deprecated": none, "id": "h2", "note": none, "stability": "experimental",
// "value": "h2"}
static constexpr const char *kH2 = "h2";
/**
 * SAP HANA.
 */
// DEBUG: {"brief": "SAP HANA", "deprecated": none, "id": "hanadb", "note": none, "stability":
// "experimental", "value": "hanadb"}
static constexpr const char *kHanadb = "hanadb";
/**
 * Apache HBase.
 */
// DEBUG: {"brief": "Apache HBase", "deprecated": none, "id": "hbase", "note": none, "stability":
// "experimental", "value": "hbase"}
static constexpr const char *kHbase = "hbase";
/**
 * Apache Hive.
 */
// DEBUG: {"brief": "Apache Hive", "deprecated": none, "id": "hive", "note": none, "stability":
// "experimental", "value": "hive"}
static constexpr const char *kHive = "hive";
/**
 * HyperSQL DataBase.
 */
// DEBUG: {"brief": "HyperSQL DataBase", "deprecated": none, "id": "hsqldb", "note": none,
// "stability": "experimental", "value": "hsqldb"}
static constexpr const char *kHsqldb = "hsqldb";
/**
 * InfluxDB.
 */
// DEBUG: {"brief": "InfluxDB", "deprecated": none, "id": "influxdb", "note": none, "stability":
// "experimental", "value": "influxdb"}
static constexpr const char *kInfluxdb = "influxdb";
/**
 * Informix.
 */
// DEBUG: {"brief": "Informix", "deprecated": none, "id": "informix", "note": none, "stability":
// "experimental", "value": "informix"}
static constexpr const char *kInformix = "informix";
/**
 * Ingres.
 */
// DEBUG: {"brief": "Ingres", "deprecated": none, "id": "ingres", "note": none, "stability":
// "experimental", "value": "ingres"}
static constexpr const char *kIngres = "ingres";
/**
 * InstantDB.
 */
// DEBUG: {"brief": "InstantDB", "deprecated": none, "id": "instantdb", "note": none, "stability":
// "experimental", "value": "instantdb"}
static constexpr const char *kInstantdb = "instantdb";
/**
 * InterBase.
 */
// DEBUG: {"brief": "InterBase", "deprecated": none, "id": "interbase", "note": none, "stability":
// "experimental", "value": "interbase"}
static constexpr const char *kInterbase = "interbase";
/**
 * MariaDB.
 */
// DEBUG: {"brief": "MariaDB", "deprecated": none, "id": "mariadb", "note": none, "stability":
// "experimental", "value": "mariadb"}
static constexpr const char *kMariadb = "mariadb";
/**
 * SAP MaxDB.
 */
// DEBUG: {"brief": "SAP MaxDB", "deprecated": none, "id": "maxdb", "note": none, "stability":
// "experimental", "value": "maxdb"}
static constexpr const char *kMaxdb = "maxdb";
/**
 * Memcached.
 */
// DEBUG: {"brief": "Memcached", "deprecated": none, "id": "memcached", "note": none, "stability":
// "experimental", "value": "memcached"}
static constexpr const char *kMemcached = "memcached";
/**
 * MongoDB.
 */
// DEBUG: {"brief": "MongoDB", "deprecated": none, "id": "mongodb", "note": none, "stability":
// "experimental", "value": "mongodb"}
static constexpr const char *kMongodb = "mongodb";
/**
 * Microsoft SQL Server.
 */
// DEBUG: {"brief": "Microsoft SQL Server", "deprecated": none, "id": "mssql", "note": none,
// "stability": "experimental", "value": "mssql"}
static constexpr const char *kMssql = "mssql";
/**
 * @Deprecated: Removed, use @code other_sql @endcode instead.
 */
// DEBUG: {"brief": "Deprecated, Microsoft SQL Server Compact is discontinued.", "deprecated":
// "Removed, use `other_sql` instead.", "id": "mssqlcompact", "note": none, "stability":
// "experimental", "value": "mssqlcompact"}
static constexpr const char *kMssqlcompact = "mssqlcompact";
/**
 * MySQL.
 */
// DEBUG: {"brief": "MySQL", "deprecated": none, "id": "mysql", "note": none, "stability":
// "experimental", "value": "mysql"}
static constexpr const char *kMysql = "mysql";
/**
 * Neo4j.
 */
// DEBUG: {"brief": "Neo4j", "deprecated": none, "id": "neo4j", "note": none, "stability":
// "experimental", "value": "neo4j"}
static constexpr const char *kNeo4j = "neo4j";
/**
 * Netezza.
 */
// DEBUG: {"brief": "Netezza", "deprecated": none, "id": "netezza", "note": none, "stability":
// "experimental", "value": "netezza"}
static constexpr const char *kNetezza = "netezza";
/**
 * OpenSearch.
 */
// DEBUG: {"brief": "OpenSearch", "deprecated": none, "id": "opensearch", "note": none, "stability":
// "experimental", "value": "opensearch"}
static constexpr const char *kOpensearch = "opensearch";
/**
 * Oracle Database.
 */
// DEBUG: {"brief": "Oracle Database", "deprecated": none, "id": "oracle", "note": none,
// "stability": "experimental", "value": "oracle"}
static constexpr const char *kOracle = "oracle";
/**
 * Pervasive PSQL.
 */
// DEBUG: {"brief": "Pervasive PSQL", "deprecated": none, "id": "pervasive", "note": none,
// "stability": "experimental", "value": "pervasive"}
static constexpr const char *kPervasive = "pervasive";
/**
 * PointBase.
 */
// DEBUG: {"brief": "PointBase", "deprecated": none, "id": "pointbase", "note": none, "stability":
// "experimental", "value": "pointbase"}
static constexpr const char *kPointbase = "pointbase";
/**
 * PostgreSQL.
 */
// DEBUG: {"brief": "PostgreSQL", "deprecated": none, "id": "postgresql", "note": none, "stability":
// "experimental", "value": "postgresql"}
static constexpr const char *kPostgresql = "postgresql";
/**
 * Progress Database.
 */
// DEBUG: {"brief": "Progress Database", "deprecated": none, "id": "progress", "note": none,
// "stability": "experimental", "value": "progress"}
static constexpr const char *kProgress = "progress";
/**
 * Redis.
 */
// DEBUG: {"brief": "Redis", "deprecated": none, "id": "redis", "note": none, "stability":
// "experimental", "value": "redis"}
static constexpr const char *kRedis = "redis";
/**
 * Amazon Redshift.
 */
// DEBUG: {"brief": "Amazon Redshift", "deprecated": none, "id": "redshift", "note": none,
// "stability": "experimental", "value": "redshift"}
static constexpr const char *kRedshift = "redshift";
/**
 * Cloud Spanner.
 */
// DEBUG: {"brief": "Cloud Spanner", "deprecated": none, "id": "spanner", "note": none, "stability":
// "experimental", "value": "spanner"}
static constexpr const char *kSpanner = "spanner";
/**
 * SQLite.
 */
// DEBUG: {"brief": "SQLite", "deprecated": none, "id": "sqlite", "note": none, "stability":
// "experimental", "value": "sqlite"}
static constexpr const char *kSqlite = "sqlite";
/**
 * Sybase.
 */
// DEBUG: {"brief": "Sybase", "deprecated": none, "id": "sybase", "note": none, "stability":
// "experimental", "value": "sybase"}
static constexpr const char *kSybase = "sybase";
/**
 * Teradata.
 */
// DEBUG: {"brief": "Teradata", "deprecated": none, "id": "teradata", "note": none, "stability":
// "experimental", "value": "teradata"}
static constexpr const char *kTeradata = "teradata";
/**
 * Trino.
 */
// DEBUG: {"brief": "Trino", "deprecated": none, "id": "trino", "note": none, "stability":
// "experimental", "value": "trino"}
static constexpr const char *kTrino = "trino";
/**
 * Vertica.
 */
// DEBUG: {"brief": "Vertica", "deprecated": none, "id": "vertica", "note": none, "stability":
// "experimental", "value": "vertica"}
static constexpr const char *kVertica = "vertica";
}  // namespace DbSystemValues

}  // namespace db
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
