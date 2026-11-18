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
namespace messaging
{

/**
  The number of messages sent, received, or processed in the scope of the batching operation.
  <p>
  Instrumentations SHOULD NOT set @code messaging.batch.message_count @endcode on spans that operate
  with a single message. When a messaging client library supports both batch and single-message API
  for the same operation, instrumentations SHOULD use @code messaging.batch.message_count @endcode
  for batching APIs and SHOULD NOT use it for single-message APIs.
 */
static constexpr const char *kMessagingBatchMessageCount = "messaging.batch.message_count";

/**
  A unique identifier for the client that consumes or produces a message.
 */
static constexpr const char *kMessagingClientId = "messaging.client.id";

/**
  The name of the consumer group with which a consumer is associated.
  <p>
  Semantic conventions for individual messaging systems SHOULD document whether @code
  messaging.consumer.group.name @endcode is applicable and what it means in the context of that
  system.
 */
static constexpr const char *kMessagingConsumerGroupName = "messaging.consumer.group.name";

/**
  A boolean that is true if the message destination is anonymous (could be unnamed or have
  auto-generated name).
 */
static constexpr const char *kMessagingDestinationAnonymous = "messaging.destination.anonymous";

/**
  The message destination name
  <p>
  SHOULD uniquely identify a specific queue, topic or other entity within the broker. If
  the broker doesn't have such notion, it SHOULD uniquely identify the broker.
 */
static constexpr const char *kMessagingDestinationName = "messaging.destination.name";

/**
  The identifier of the partition messages are sent to or received from, unique within the @code
  messaging.destination.name @endcode.
 */
static constexpr const char *kMessagingDestinationPartitionId =
    "messaging.destination.partition.id";

/**
  The name of the destination subscription from which a message is consumed.
  <p>
  Semantic conventions for individual messaging systems SHOULD document whether @code
  messaging.destination.subscription.name @endcode is applicable and what it means in the context of
  that system.
 */
static constexpr const char *kMessagingDestinationSubscriptionName =
    "messaging.destination.subscription.name";

/**
  Low cardinality representation of the messaging destination name
  <p>
  Destination names could be constructed from templates. An example would be a destination name
  involving a username or product ID. Although the destination name in this case is of high
  cardinality, the underlying template is of low cardinality and can be effectively used for
  grouping and aggregation.
 */
static constexpr const char *kMessagingDestinationTemplate = "messaging.destination.template";

/**
  A boolean that is true if the message destination is temporary and might not exist anymore after
  messages are processed.
 */
static constexpr const char *kMessagingDestinationTemporary = "messaging.destination.temporary";

/**
  Deprecated, no replacement at this time.

  @deprecated
  {"note": "Removed. No replacement at this time.", "reason": "obsoleted"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingDestinationPublishAnonymous =
    "messaging.destination_publish.anonymous";

/**
  Deprecated, no replacement at this time.

  @deprecated
  {"note": "Removed. No replacement at this time.", "reason": "obsoleted"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingDestinationPublishName =
    "messaging.destination_publish.name";

/**
  Deprecated, use @code messaging.consumer.group.name @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.consumer.group.name @endcode.", "reason": "renamed",
  "renamed_to": "messaging.consumer.group.name"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingEventhubsConsumerGroup =
    "messaging.eventhubs.consumer.group";

/**
  Deprecated, use @code azure.eventhubs.message.enqueued_time @endcode instead.

  @deprecated
  {"note": "Replaced by @code azure.eventhubs.message.enqueued_time @endcode.", "reason": "renamed",
  "renamed_to": "azure.eventhubs.message.enqueued_time"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingEventhubsMessageEnqueuedTime =
    "messaging.eventhubs.message.enqueued_time";

/**
  Deprecated, use @code gcp.pubsub.message.ack_deadline @endcode instead.

  @deprecated
  {"note": "Replaced by @code gcp.pubsub.message.ack_deadline @endcode.", "reason": "renamed",
  "renamed_to": "gcp.pubsub.message.ack_deadline"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingGcpPubsubMessageAckDeadline =
    "messaging.gcp_pubsub.message.ack_deadline";

/**
  Deprecated, use @code gcp.pubsub.message.ack_id @endcode instead.

  @deprecated
  {"note": "Replaced by @code gcp.pubsub.message.ack_id @endcode.", "reason": "renamed",
  "renamed_to": "gcp.pubsub.message.ack_id"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingGcpPubsubMessageAckId =
    "messaging.gcp_pubsub.message.ack_id";

/**
  Deprecated, use @code gcp.pubsub.message.delivery_attempt @endcode instead.

  @deprecated
  {"note": "Replaced by @code gcp.pubsub.message.delivery_attempt @endcode.", "reason": "renamed",
  "renamed_to": "gcp.pubsub.message.delivery_attempt"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingGcpPubsubMessageDeliveryAttempt =
    "messaging.gcp_pubsub.message.delivery_attempt";

/**
  Deprecated, use @code gcp.pubsub.message.ordering_key @endcode instead.

  @deprecated
  {"note": "Replaced by @code gcp.pubsub.message.ordering_key @endcode.", "reason": "renamed",
  "renamed_to": "gcp.pubsub.message.ordering_key"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingGcpPubsubMessageOrderingKey =
    "messaging.gcp_pubsub.message.ordering_key";

/**
  Deprecated, use @code kafka.cluster.id @endcode instead.

  @deprecated
  {"note": "Replaced by @code kafka.cluster.id @endcode.", "reason": "renamed", "renamed_to":
  "kafka.cluster.id"} <p> The cluster ID is a unique identifier reported by the Kafka broker. It
  identifies the cluster independently of the individual brokers the client is configured to connect
  to, and remains stable even if broker hostnames, IP addresses, or ports change.
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaClusterId =
    "messaging.kafka.cluster.id";

/**
  Deprecated, use @code messaging.consumer.group.name @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.consumer.group.name @endcode.", "reason": "renamed",
  "renamed_to": "messaging.consumer.group.name"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaConsumerGroup =
    "messaging.kafka.consumer.group";

/**
  Deprecated, use @code messaging.destination.partition.id @endcode instead.

  @deprecated
  {"note": "Record string representation of the partition ID in @code
  messaging.destination.partition.id @endcode attribute.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaDestinationPartition =
    "messaging.kafka.destination.partition";

/**
  Deprecated, use @code kafka.message.key @endcode instead.

  @deprecated
  {"note": "Replaced by @code kafka.message.key @endcode.", "reason": "renamed", "renamed_to":
  "kafka.message.key"} <p> If the key type is not string, it's string representation has to be
  supplied for the attribute. If the key has no unambiguous, canonical string form, don't include
  its value.
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaMessageKey =
    "messaging.kafka.message.key";

/**
  Deprecated, use @code kafka.offset @endcode instead.

  @deprecated
  {"note": "Replaced by @code kafka.offset @endcode.", "reason": "renamed", "renamed_to":
  "kafka.offset"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaMessageOffset =
    "messaging.kafka.message.offset";

/**
  Deprecated, use @code kafka.message.tombstone @endcode instead.

  @deprecated
  {"note": "Replaced by @code kafka.message.tombstone @endcode.", "reason": "renamed", "renamed_to":
  "kafka.message.tombstone"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaMessageTombstone =
    "messaging.kafka.message.tombstone";

/**
  Deprecated, use @code kafka.offset @endcode instead.

  @deprecated
  {"note": "Replaced by @code kafka.offset @endcode.", "reason": "renamed", "renamed_to":
  "kafka.offset"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingKafkaOffset =
    "messaging.kafka.offset";

/**
  The size of the message body in bytes.
  <p>
  This can refer to both the compressed or uncompressed body size. If both sizes are known, the
  uncompressed body size should be used.
 */
static constexpr const char *kMessagingMessageBodySize = "messaging.message.body.size";

/**
  The conversation ID identifying the conversation to which the message belongs, represented as a
  string. Sometimes called "Correlation ID".
 */
static constexpr const char *kMessagingMessageConversationId = "messaging.message.conversation_id";

/**
  The size of the message body and metadata in bytes.
  <p>
  This can refer to both the compressed or uncompressed size. If both sizes are known, the
  uncompressed size should be used.
 */
static constexpr const char *kMessagingMessageEnvelopeSize = "messaging.message.envelope.size";

/**
  A value used by the messaging system as an identifier for the message, represented as a string.
 */
static constexpr const char *kMessagingMessageId = "messaging.message.id";

/**
  Deprecated, use @code messaging.operation.type @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.operation.type @endcode.", "reason": "renamed",
  "renamed_to": "messaging.operation.type"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingOperation = "messaging.operation";

/**
  The system-specific name of the messaging operation.
 */
static constexpr const char *kMessagingOperationName = "messaging.operation.name";

/**
  A string identifying the type of the messaging operation.
  <p>
  If a custom value is used, it MUST be of low cardinality.
 */
static constexpr const char *kMessagingOperationType = "messaging.operation.type";

/**
  Deprecated, use @code rabbitmq.destination.routing_key @endcode instead.

  @deprecated
  {"note": "Replaced by @code rabbitmq.destination.routing_key @endcode.", "reason": "renamed",
  "renamed_to": "rabbitmq.destination.routing_key"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRabbitmqDestinationRoutingKey =
    "messaging.rabbitmq.destination.routing_key";

/**
  Deprecated, use @code rabbitmq.message.delivery_tag @endcode instead.

  @deprecated
  {"note": "Replaced by @code rabbitmq.message.delivery_tag @endcode.", "reason": "renamed",
  "renamed_to": "rabbitmq.message.delivery_tag"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRabbitmqMessageDeliveryTag =
    "messaging.rabbitmq.message.delivery_tag";

/**
  Deprecated, use @code messaging.consumer.group.name @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.consumer.group.name @endcode on the consumer spans. No
  replacement for producer spans.\n", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqClientGroup =
    "messaging.rocketmq.client_group";

/**
  Deprecated, use @code rocketmq.consumption_model @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.consumption_model @endcode.", "reason": "renamed",
  "renamed_to": "rocketmq.consumption_model"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqConsumptionModel =
    "messaging.rocketmq.consumption_model";

/**
  Deprecated, use @code rocketmq.message.delay_time_level @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.delay_time_level @endcode.", "reason": "renamed",
  "renamed_to": "rocketmq.message.delay_time_level"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageDelayTimeLevel =
    "messaging.rocketmq.message.delay_time_level";

/**
  Deprecated, use @code rocketmq.message.delivery_timestamp @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.delivery_timestamp @endcode.", "reason": "renamed",
  "renamed_to": "rocketmq.message.delivery_timestamp"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageDeliveryTimestamp =
    "messaging.rocketmq.message.delivery_timestamp";

/**
  Deprecated, use @code rocketmq.message.group @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.group @endcode.", "reason": "renamed", "renamed_to":
  "rocketmq.message.group"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageGroup =
    "messaging.rocketmq.message.group";

/**
  Deprecated, use @code rocketmq.message.keys @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.keys @endcode.", "reason": "renamed", "renamed_to":
  "rocketmq.message.keys"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageKeys =
    "messaging.rocketmq.message.keys";

/**
  Deprecated, use @code rocketmq.message.tag @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.tag @endcode.", "reason": "renamed", "renamed_to":
  "rocketmq.message.tag"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageTag =
    "messaging.rocketmq.message.tag";

/**
  Deprecated, use @code rocketmq.message.type @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.message.type @endcode.", "reason": "renamed", "renamed_to":
  "rocketmq.message.type"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqMessageType =
    "messaging.rocketmq.message.type";

/**
  Deprecated, use @code rocketmq.namespace @endcode instead.

  @deprecated
  {"note": "Replaced by @code rocketmq.namespace @endcode.", "reason": "renamed", "renamed_to":
  "rocketmq.namespace"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingRocketmqNamespace =
    "messaging.rocketmq.namespace";

/**
  Deprecated, use @code messaging.destination.subscription.name @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.destination.subscription.name @endcode.", "reason":
  "renamed", "renamed_to": "messaging.destination.subscription.name"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char
    *kMessagingServicebusDestinationSubscriptionName =
        "messaging.servicebus.destination.subscription_name";

/**
  Deprecated, use @code azure.servicebus.disposition_status @endcode instead.

  @deprecated
  {"note": "Replaced by @code azure.servicebus.disposition_status @endcode.", "reason": "renamed",
  "renamed_to": "azure.servicebus.disposition_status"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingServicebusDispositionStatus =
    "messaging.servicebus.disposition_status";

/**
  Deprecated, use @code azure.servicebus.message.delivery_count @endcode instead.

  @deprecated
  {"note": "Replaced by @code azure.servicebus.message.delivery_count @endcode.", "reason":
  "renamed", "renamed_to": "azure.servicebus.message.delivery_count"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingServicebusMessageDeliveryCount =
    "messaging.servicebus.message.delivery_count";

/**
  Deprecated, use @code azure.servicebus.message.enqueued_time @endcode instead.

  @deprecated
  {"note": "Replaced by @code azure.servicebus.message.enqueued_time @endcode.", "reason":
  "renamed", "renamed_to": "azure.servicebus.message.enqueued_time"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingServicebusMessageEnqueuedTime =
    "messaging.servicebus.message.enqueued_time";

/**
  Deprecated, use @code messaging.system.name @endcode instead.

  @deprecated
  {"note": "Replaced by @code messaging.system.name @endcode.", "reason": "renamed", "renamed_to":
  "messaging.system.name"} <p> The actual messaging system may differ from the one known by the
  client. For example, when using Kafka client libraries to communicate with Azure Event Hubs, the
  @code messaging.system @endcode is set to @code kafka @endcode based on the instrumentation's best
  knowledge. <p> When migrating to @code messaging.system.name @endcode, update the following
  values: <ul> <li>@code aws_sqs @endcode to @code aws.sqs @endcode</li> <li>@code eventgrid
  @endcode to @code azure.eventgrid @endcode</li> <li>@code eventhubs @endcode to @code
  azure.eventhubs @endcode</li> <li>@code servicebus @endcode to @code azure.servicebus
  @endcode</li> <li>@code gcp_pubsub @endcode to @code gcp.pubsub @endcode</li>
  </ul>
  <p>
  All other values are unchanged.
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kMessagingSystem = "messaging.system";

/**
  The messaging system as identified by the client instrumentation.
  <p>
  The actual messaging system may differ from the one known by the client. For example, when using
  Kafka client libraries to communicate with Azure Event Hubs, the @code messaging.system.name
  @endcode is set to @code kafka @endcode based on the instrumentation's best knowledge.
 */
static constexpr const char *kMessagingSystemName = "messaging.system.name";

namespace MessagingOperationTypeValues
{
/**
  A message is created. "Create" spans always refer to a single message and are used to provide a
  unique creation context for messages in batch sending scenarios.
 */
static constexpr const char *kCreate = "create";

/**
  One or more messages are provided for sending to an intermediary. If a single message is sent, the
  context of the "Send" span can be used as the creation context and no "Create" span needs to be
  created.
 */
static constexpr const char *kSend = "send";

/**
  One or more messages are requested by a consumer. This operation refers to pull-based scenarios,
  where consumers explicitly call methods of messaging SDKs to receive messages.
 */
static constexpr const char *kReceive = "receive";

/**
  One or more messages are processed by a consumer.
 */
static constexpr const char *kProcess = "process";

/**
  One or more messages are settled.
 */
static constexpr const char *kSettle = "settle";

/**
  Deprecated. Use @code process @endcode instead.

  @deprecated
  {"note": "Replaced by @code process @endcode.", "reason": "renamed", "renamed_to": "process"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kDeliver = "deliver";

/**
  Deprecated. Use @code send @endcode instead.

  @deprecated
  {"note": "Replaced by @code send @endcode.", "reason": "renamed", "renamed_to": "send"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kPublish = "publish";

}  // namespace MessagingOperationTypeValues

namespace MessagingRocketmqConsumptionModelValues
{
/**
  Clustering consumption model
 */
static constexpr const char *kClustering = "clustering";

/**
  Broadcasting consumption model
 */
static constexpr const char *kBroadcasting = "broadcasting";

}  // namespace MessagingRocketmqConsumptionModelValues

namespace MessagingRocketmqMessageTypeValues
{
/**
  Normal message
 */
static constexpr const char *kNormal = "normal";

/**
  FIFO message
 */
static constexpr const char *kFifo = "fifo";

/**
  Delay message
 */
static constexpr const char *kDelay = "delay";

/**
  Transaction message
 */
static constexpr const char *kTransaction = "transaction";

}  // namespace MessagingRocketmqMessageTypeValues

namespace MessagingServicebusDispositionStatusValues
{
/**
  Message is completed
 */
static constexpr const char *kComplete = "complete";

/**
  Message is abandoned
 */
static constexpr const char *kAbandon = "abandon";

/**
  Message is sent to dead letter queue
 */
static constexpr const char *kDeadLetter = "dead_letter";

/**
  Message is deferred
 */
static constexpr const char *kDefer = "defer";

}  // namespace MessagingServicebusDispositionStatusValues

namespace MessagingSystemValues
{
/**
  Apache ActiveMQ
 */
static constexpr const char *kActivemq = "activemq";

/**
  Amazon Simple Notification Service (SNS)
 */
static constexpr const char *kAwsSns = "aws.sns";

/**
  Amazon Simple Queue Service (SQS)
 */
static constexpr const char *kAwsSqs = "aws_sqs";

/**
  Azure Event Grid
 */
static constexpr const char *kEventgrid = "eventgrid";

/**
  Azure Event Hubs
 */
static constexpr const char *kEventhubs = "eventhubs";

/**
  Azure Service Bus
 */
static constexpr const char *kServicebus = "servicebus";

/**
  Google Cloud Pub/Sub
 */
static constexpr const char *kGcpPubsub = "gcp_pubsub";

/**
  Java Message Service
 */
static constexpr const char *kJms = "jms";

/**
  Apache Kafka
 */
static constexpr const char *kKafka = "kafka";

/**
  RabbitMQ
 */
static constexpr const char *kRabbitmq = "rabbitmq";

/**
  Apache RocketMQ
 */
static constexpr const char *kRocketmq = "rocketmq";

/**
  Apache Pulsar
 */
static constexpr const char *kPulsar = "pulsar";

}  // namespace MessagingSystemValues

namespace MessagingSystemNameValues
{
/**
  Apache ActiveMQ
 */
static constexpr const char *kActivemq = "activemq";

/**
  Amazon Simple Notification Service (SNS)
 */
static constexpr const char *kAwsSns = "aws.sns";

/**
  Amazon Simple Queue Service (SQS)
 */
static constexpr const char *kAwsSqs = "aws.sqs";

/**
  Azure Event Grid
 */
static constexpr const char *kAzureEventgrid = "azure.eventgrid";

/**
  Azure Event Hubs
 */
static constexpr const char *kAzureEventhubs = "azure.eventhubs";

/**
  Azure Service Bus
 */
static constexpr const char *kAzureServicebus = "azure.servicebus";

/**
  Google Cloud Pub/Sub
 */
static constexpr const char *kGcpPubsub = "gcp.pubsub";

/**
  Java Message Service
 */
static constexpr const char *kJms = "jms";

/**
  Apache Kafka
 */
static constexpr const char *kKafka = "kafka";

/**
  RabbitMQ
 */
static constexpr const char *kRabbitmq = "rabbitmq";

/**
  Apache RocketMQ
 */
static constexpr const char *kRocketmq = "rocketmq";

/**
  Apache Pulsar
 */
static constexpr const char *kPulsar = "pulsar";

}  // namespace MessagingSystemNameValues

}  // namespace messaging
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
