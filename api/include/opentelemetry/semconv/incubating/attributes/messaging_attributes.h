

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
 * The number of messages sent, received, or processed in the scope of the batching operation.
 * Note: Instrumentations SHOULD NOT set @code messaging.batch.message_count @endcode on spans that
 * operate with a single message. When a messaging client library supports both batch and
 * single-message API for the same operation, instrumentations SHOULD use @code
 * messaging.batch.message_count @endcode for batching APIs and SHOULD NOT use it for single-message
 * APIs.
 */
static const char *kMessagingBatchMessageCount = "messaging.batch.message_count";

/**
 * A unique identifier for the client that consumes or produces a message.
 */
static const char *kMessagingClientId = "messaging.client.id";

// /**
*@Deprecated : Replaced by @code messaging.client.id @endcode.* /
    // static const char *kMessagingClientId = "messaging.client_id";

    /**
     * The name of the consumer group with which a consumer is associated.
     * Note: Semantic conventions for individual messaging systems SHOULD document whether @code
     * messaging.consumer.group.name @endcode is applicable and what it means in the context of that
     * system.
     */
    static const char *kMessagingConsumerGroupName = "messaging.consumer.group.name";

/**
 * A boolean that is true if the message destination is anonymous (could be unnamed or have
 * auto-generated name).
 */
static const char *kMessagingDestinationAnonymous = "messaging.destination.anonymous";

/**
 * The message destination name.
 * Note: Destination name SHOULD uniquely identify a specific queue, topic or other entity within
 * the broker. If the broker doesn't have such notion, the destination name SHOULD uniquely identify
 * the broker.
 */
static const char *kMessagingDestinationName = "messaging.destination.name";

/**
 * The identifier of the partition messages are sent to or received from, unique within the @code
 * messaging.destination.name @endcode.
 */
static const char *kMessagingDestinationPartitionId = "messaging.destination.partition.id";

/**
 * The name of the destination subscription from which a message is consumed.
 * Note: Semantic conventions for individual messaging systems SHOULD document whether @code
 * messaging.destination.subscription.name @endcode is applicable and what it means in the context
 * of that system.
 */
static const char *kMessagingDestinationSubscriptionName =
    "messaging.destination.subscription.name";

/**
 * Low cardinality representation of the messaging destination name.
 * Note: Destination names could be constructed from templates. An example would be a destination
 * name involving a user name or product id. Although the destination name in this case is of high
 * cardinality, the underlying template is of low cardinality and can be effectively used for
 * grouping and aggregation.
 */
static const char *kMessagingDestinationTemplate = "messaging.destination.template";

/**
 * A boolean that is true if the message destination is temporary and might not exist anymore after
 * messages are processed.
 */
static const char *kMessagingDestinationTemporary = "messaging.destination.temporary";

/**
 * @Deprecated: No replacement at this time.
 */
static const char *kMessagingDestinationPublishAnonymous =
    "messaging.destination_publish.anonymous";

/**
 * @Deprecated: No replacement at this time.
 */
static const char *kMessagingDestinationPublishName = "messaging.destination_publish.name";

/**
 * @Deprecated: Replaced by @code messaging.consumer.group.name @endcode.
 */
static const char *kMessagingEventhubsConsumerGroup = "messaging.eventhubs.consumer.group";

/**
 * The UTC epoch seconds at which the message has been accepted and stored in the entity.
 */
static const char *kMessagingEventhubsMessageEnqueuedTime =
    "messaging.eventhubs.message.enqueued_time";

/**
 * The ack deadline in seconds set for the modify ack deadline request.
 */
static const char *kMessagingGcpPubsubMessageAckDeadline =
    "messaging.gcp_pubsub.message.ack_deadline";

/**
 * The ack id for a given message.
 */
static const char *kMessagingGcpPubsubMessageAckId = "messaging.gcp_pubsub.message.ack_id";

/**
 * The delivery attempt for a given message.
 */
static const char *kMessagingGcpPubsubMessageDeliveryAttempt =
    "messaging.gcp_pubsub.message.delivery_attempt";

/**
 * The ordering key for a given message. If the attribute is not present, the message does not have
 * an ordering key.
 */
static const char *kMessagingGcpPubsubMessageOrderingKey =
    "messaging.gcp_pubsub.message.ordering_key";

/**
 * @Deprecated: Replaced by @code messaging.consumer.group.name @endcode.
 */
static const char *kMessagingKafkaConsumerGroup = "messaging.kafka.consumer.group";

/**
 * @Deprecated: Replaced by @code messaging.destination.partition.id @endcode.
 */
static const char *kMessagingKafkaDestinationPartition = "messaging.kafka.destination.partition";

/**
 * Message keys in Kafka are used for grouping alike messages to ensure they're processed on the
 * same partition. They differ from @code messaging.message.id @endcode in that they're not unique.
 * If the key is @code null @endcode, the attribute MUST NOT be set. Note: If the key type is not
 * string, it's string representation has to be supplied for the attribute. If the key has no
 * unambiguous, canonical string form, don't include its value.
 */
static const char *kMessagingKafkaMessageKey = "messaging.kafka.message.key";

/**
 * @Deprecated: Replaced by @code messaging.kafka.offset @endcode.
 */
static const char *kMessagingKafkaMessageOffset = "messaging.kafka.message.offset";

/**
 * A boolean that is true if the message is a tombstone.
 */
static const char *kMessagingKafkaMessageTombstone = "messaging.kafka.message.tombstone";

/**
 * The offset of a record in the corresponding Kafka partition.
 */
static const char *kMessagingKafkaOffset = "messaging.kafka.offset";

/**
 * The size of the message body in bytes.
 * Note: This can refer to both the compressed or uncompressed body size. If both sizes are known,
 * the uncompressed body size should be used.
 */
static const char *kMessagingMessageBodySize = "messaging.message.body.size";

/**
 * The conversation ID identifying the conversation to which the message belongs, represented as a
 * string. Sometimes called "Correlation ID".
 */
static const char *kMessagingMessageConversationId = "messaging.message.conversation_id";

/**
 * The size of the message body and metadata in bytes.
 * Note: This can refer to both the compressed or uncompressed size. If both sizes are known, the
 * uncompressed size should be used.
 */
static const char *kMessagingMessageEnvelopeSize = "messaging.message.envelope.size";

/**
 * A value used by the messaging system as an identifier for the message, represented as a string.
 */
static const char *kMessagingMessageId = "messaging.message.id";

/**
 * @Deprecated: Replaced by @code messaging.operation.type @endcode.
 */
static const char *kMessagingOperation = "messaging.operation";

/**
 * The system-specific name of the messaging operation.
 */
static const char *kMessagingOperationName = "messaging.operation.name";

/**
 * A string identifying the type of the messaging operation.
 * Note: If a custom value is used, it MUST be of low cardinality.
 */
static const char *kMessagingOperationType = "messaging.operation.type";

/**
 * RabbitMQ message routing key.
 */
static const char *kMessagingRabbitmqDestinationRoutingKey =
    "messaging.rabbitmq.destination.routing_key";

/**
 * RabbitMQ message delivery tag.
 */
static const char *kMessagingRabbitmqMessageDeliveryTag = "messaging.rabbitmq.message.delivery_tag";

/**
 * @Deprecated: Replaced by @code messaging.consumer.group.name @endcode on the consumer spans. No
 * replacement for producer spans.
 */
static const char *kMessagingRocketmqClientGroup = "messaging.rocketmq.client_group";

/**
 * Model of message consumption. This only applies to consumer spans.
 */
static const char *kMessagingRocketmqConsumptionModel = "messaging.rocketmq.consumption_model";

/**
 * The delay time level for delay message, which determines the message delay time.
 */
static const char *kMessagingRocketmqMessageDelayTimeLevel =
    "messaging.rocketmq.message.delay_time_level";

/**
 * The timestamp in milliseconds that the delay message is expected to be delivered to consumer.
 */
static const char *kMessagingRocketmqMessageDeliveryTimestamp =
    "messaging.rocketmq.message.delivery_timestamp";

/**
 * It is essential for FIFO message. Messages that belong to the same message group are always
 * processed one by one within the same consumer group.
 */
static const char *kMessagingRocketmqMessageGroup = "messaging.rocketmq.message.group";

/**
 * Key(s) of message, another way to mark message besides message id.
 */
static const char *kMessagingRocketmqMessageKeys = "messaging.rocketmq.message.keys";

/**
 * The secondary classifier of message besides topic.
 */
static const char *kMessagingRocketmqMessageTag = "messaging.rocketmq.message.tag";

/**
 * Type of message.
 */
static const char *kMessagingRocketmqMessageType = "messaging.rocketmq.message.type";

/**
 * Namespace of RocketMQ resources, resources in different namespaces are individual.
 */
static const char *kMessagingRocketmqNamespace = "messaging.rocketmq.namespace";

/**
 * @Deprecated: Replaced by @code messaging.servicebus.destination.subscription_name @endcode.
 */
static const char *kMessagingServicebusDestinationSubscriptionName =
    "messaging.servicebus.destination.subscription_name";

/**
 * Describes the <a
 * href="https://learn.microsoft.com/azure/service-bus-messaging/message-transfers-locks-settlement#peeklock">settlement
 * type</a>.
 */
static const char *kMessagingServicebusDispositionStatus =
    "messaging.servicebus.disposition_status";

/**
 * Number of deliveries that have been attempted for this message.
 */
static const char *kMessagingServicebusMessageDeliveryCount =
    "messaging.servicebus.message.delivery_count";

/**
 * The UTC epoch seconds at which the message has been accepted and stored in the entity.
 */
static const char *kMessagingServicebusMessageEnqueuedTime =
    "messaging.servicebus.message.enqueued_time";

/**
 * The messaging system as identified by the client instrumentation.
 * Note: The actual messaging system may differ from the one known by the client. For example, when
 * using Kafka client libraries to communicate with Azure Event Hubs, the @code messaging.system
 * @endcode is set to @code kafka @endcode based on the instrumentation's best knowledge.
 */
static const char *kMessagingSystem = "messaging.system";

// DEBUG: {"brief": "A string identifying the type of the messaging operation.\n", "name":
// "messaging.operation.type", "note": "If a custom value is used, it MUST be of low cardinality.",
// "requirement_level": "recommended", "root_namespace": "messaging", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "One or more messages are provided
// for publishing to an intermediary. If a single message is published, the context of the
// \"Publish\" span can be used as the creation context and no \"Create\" span needs to be
// created.\n", "deprecated": none, "id": "publish", "note": none, "stability": "experimental",
// "value": "publish"}, {"brief": "A message is created. \"Create\" spans always refer to a single
// message and are used to provide a unique creation context for messages in batch publishing
// scenarios.\n", "deprecated": none, "id": "create", "note": none, "stability": "experimental",
// "value": "create"}, {"brief": "One or more messages are requested by a consumer. This operation
// refers to pull-based scenarios, where consumers explicitly call methods of messaging SDKs to
// receive messages.\n", "deprecated": none, "id": "receive", "note": none, "stability":
// "experimental", "value": "receive"}, {"brief": "One or more messages are processed by a
// consumer.\n", "deprecated": none, "id": "process", "note": none, "stability": "experimental",
// "value": "process"}, {"brief": "One or more messages are settled.\n", "deprecated": none, "id":
// "settle", "note": none, "stability": "experimental", "value": "settle"}, {"brief": "Deprecated.
// Use `process` instead.", "deprecated": "Replaced by `process`.", "id": "deliver", "note": none,
// "stability": "experimental", "value": "deliver"}]}}
namespace MessagingOperationTypeValues
{
/**
 * One or more messages are provided for publishing to an intermediary. If a single message is
 * published, the context of the "Publish" span can be used as the creation context and no "Create"
 * span needs to be created.
 */
// DEBUG: {"brief": "One or more messages are provided for publishing to an intermediary. If a
// single message is published, the context of the \"Publish\" span can be used as the creation
// context and no \"Create\" span needs to be created.\n", "deprecated": none, "id": "publish",
// "note": none, "stability": "experimental", "value": "publish"}
static constexpr const char *kPublish = "publish";
/**
 * A message is created. "Create" spans always refer to a single message and are used to provide a
 * unique creation context for messages in batch publishing scenarios.
 */
// DEBUG: {"brief": "A message is created. \"Create\" spans always refer to a single message and are
// used to provide a unique creation context for messages in batch publishing scenarios.\n",
// "deprecated": none, "id": "create", "note": none, "stability": "experimental", "value": "create"}
static constexpr const char *kCreate = "create";
/**
 * One or more messages are requested by a consumer. This operation refers to pull-based scenarios,
 * where consumers explicitly call methods of messaging SDKs to receive messages.
 */
// DEBUG: {"brief": "One or more messages are requested by a consumer. This operation refers to
// pull-based scenarios, where consumers explicitly call methods of messaging SDKs to receive
// messages.\n", "deprecated": none, "id": "receive", "note": none, "stability": "experimental",
// "value": "receive"}
static constexpr const char *kReceive = "receive";
/**
 * One or more messages are processed by a consumer.
 */
// DEBUG: {"brief": "One or more messages are processed by a consumer.\n", "deprecated": none, "id":
// "process", "note": none, "stability": "experimental", "value": "process"}
static constexpr const char *kProcess = "process";
/**
 * One or more messages are settled.
 */
// DEBUG: {"brief": "One or more messages are settled.\n", "deprecated": none, "id": "settle",
// "note": none, "stability": "experimental", "value": "settle"}
static constexpr const char *kSettle = "settle";
/**
 * @Deprecated: Replaced by @code process @endcode.
 */
// DEBUG: {"brief": "Deprecated. Use `process` instead.", "deprecated": "Replaced by `process`.",
// "id": "deliver", "note": none, "stability": "experimental", "value": "deliver"}
static constexpr const char *kDeliver = "deliver";
}  // namespace MessagingOperationTypeValues

// DEBUG: {"brief": "Model of message consumption. This only applies to consumer spans.\n", "name":
// "messaging.rocketmq.consumption_model", "requirement_level": "recommended", "root_namespace":
// "messaging", "stability": "experimental", "type": {"allow_custom_values": true, "members":
// [{"brief": "Clustering consumption model", "deprecated": none, "id": "clustering", "note": none,
// "stability": "experimental", "value": "clustering"}, {"brief": "Broadcasting consumption model",
// "deprecated": none, "id": "broadcasting", "note": none, "stability": "experimental", "value":
// "broadcasting"}]}}
namespace MessagingRocketmqConsumptionModelValues
{
/**
 * Clustering consumption model.
 */
// DEBUG: {"brief": "Clustering consumption model", "deprecated": none, "id": "clustering", "note":
// none, "stability": "experimental", "value": "clustering"}
static constexpr const char *kClustering = "clustering";
/**
 * Broadcasting consumption model.
 */
// DEBUG: {"brief": "Broadcasting consumption model", "deprecated": none, "id": "broadcasting",
// "note": none, "stability": "experimental", "value": "broadcasting"}
static constexpr const char *kBroadcasting = "broadcasting";
}  // namespace MessagingRocketmqConsumptionModelValues

// DEBUG: {"brief": "Type of message.\n", "name": "messaging.rocketmq.message.type",
// "requirement_level": "recommended", "root_namespace": "messaging", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "Normal message", "deprecated": none,
// "id": "normal", "note": none, "stability": "experimental", "value": "normal"}, {"brief": "FIFO
// message", "deprecated": none, "id": "fifo", "note": none, "stability": "experimental", "value":
// "fifo"}, {"brief": "Delay message", "deprecated": none, "id": "delay", "note": none, "stability":
// "experimental", "value": "delay"}, {"brief": "Transaction message", "deprecated": none, "id":
// "transaction", "note": none, "stability": "experimental", "value": "transaction"}]}}
namespace MessagingRocketmqMessageTypeValues
{
/**
 * Normal message.
 */
// DEBUG: {"brief": "Normal message", "deprecated": none, "id": "normal", "note": none, "stability":
// "experimental", "value": "normal"}
static constexpr const char *kNormal = "normal";
/**
 * FIFO message.
 */
// DEBUG: {"brief": "FIFO message", "deprecated": none, "id": "fifo", "note": none, "stability":
// "experimental", "value": "fifo"}
static constexpr const char *kFifo = "fifo";
/**
 * Delay message.
 */
// DEBUG: {"brief": "Delay message", "deprecated": none, "id": "delay", "note": none, "stability":
// "experimental", "value": "delay"}
static constexpr const char *kDelay = "delay";
/**
 * Transaction message.
 */
// DEBUG: {"brief": "Transaction message", "deprecated": none, "id": "transaction", "note": none,
// "stability": "experimental", "value": "transaction"}
static constexpr const char *kTransaction = "transaction";
}  // namespace MessagingRocketmqMessageTypeValues

// DEBUG: {"brief": "Describes the [settlement
// type](https://learn.microsoft.com/azure/service-bus-messaging/message-transfers-locks-settlement#peeklock).\n",
// "name": "messaging.servicebus.disposition_status", "requirement_level": "recommended",
// "root_namespace": "messaging", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": "Message is completed", "deprecated": none, "id": "complete", "note": none,
// "stability": "experimental", "value": "complete"}, {"brief": "Message is abandoned",
// "deprecated": none, "id": "abandon", "note": none, "stability": "experimental", "value":
// "abandon"}, {"brief": "Message is sent to dead letter queue", "deprecated": none, "id":
// "dead_letter", "note": none, "stability": "experimental", "value": "dead_letter"}, {"brief":
// "Message is deferred", "deprecated": none, "id": "defer", "note": none, "stability":
// "experimental", "value": "defer"}]}}
namespace MessagingServicebusDispositionStatusValues
{
/**
 * Message is completed.
 */
// DEBUG: {"brief": "Message is completed", "deprecated": none, "id": "complete", "note": none,
// "stability": "experimental", "value": "complete"}
static constexpr const char *kComplete = "complete";
/**
 * Message is abandoned.
 */
// DEBUG: {"brief": "Message is abandoned", "deprecated": none, "id": "abandon", "note": none,
// "stability": "experimental", "value": "abandon"}
static constexpr const char *kAbandon = "abandon";
/**
 * Message is sent to dead letter queue.
 */
// DEBUG: {"brief": "Message is sent to dead letter queue", "deprecated": none, "id": "dead_letter",
// "note": none, "stability": "experimental", "value": "dead_letter"}
static constexpr const char *kDeadLetter = "dead_letter";
/**
 * Message is deferred.
 */
// DEBUG: {"brief": "Message is deferred", "deprecated": none, "id": "defer", "note": none,
// "stability": "experimental", "value": "defer"}
static constexpr const char *kDefer = "defer";
}  // namespace MessagingServicebusDispositionStatusValues

// DEBUG: {"brief": "The messaging system as identified by the client instrumentation.", "name":
// "messaging.system", "note": "The actual messaging system may differ from the one known by the
// client. For example, when using Kafka client libraries to communicate with Azure Event Hubs, the
// `messaging.system` is set to `kafka` based on the instrumentation's best knowledge.\n",
// "requirement_level": "recommended", "root_namespace": "messaging", "stability": "experimental",
// "type": {"allow_custom_values": true, "members": [{"brief": "Apache ActiveMQ", "deprecated":
// none, "id": "activemq", "note": none, "stability": "experimental", "value": "activemq"},
// {"brief": "Amazon Simple Queue Service (SQS)", "deprecated": none, "id": "aws_sqs", "note": none,
// "stability": "experimental", "value": "aws_sqs"}, {"brief": "Azure Event Grid", "deprecated":
// none, "id": "eventgrid", "note": none, "stability": "experimental", "value": "eventgrid"},
// {"brief": "Azure Event Hubs", "deprecated": none, "id": "eventhubs", "note": none, "stability":
// "experimental", "value": "eventhubs"}, {"brief": "Azure Service Bus", "deprecated": none, "id":
// "servicebus", "note": none, "stability": "experimental", "value": "servicebus"}, {"brief":
// "Google Cloud Pub/Sub", "deprecated": none, "id": "gcp_pubsub", "note": none, "stability":
// "experimental", "value": "gcp_pubsub"}, {"brief": "Java Message Service", "deprecated": none,
// "id": "jms", "note": none, "stability": "experimental", "value": "jms"}, {"brief": "Apache
// Kafka", "deprecated": none, "id": "kafka", "note": none, "stability": "experimental", "value":
// "kafka"}, {"brief": "RabbitMQ", "deprecated": none, "id": "rabbitmq", "note": none, "stability":
// "experimental", "value": "rabbitmq"}, {"brief": "Apache RocketMQ", "deprecated": none, "id":
// "rocketmq", "note": none, "stability": "experimental", "value": "rocketmq"}, {"brief": "Apache
// Pulsar", "deprecated": none, "id": "pulsar", "note": none, "stability": "experimental", "value":
// "pulsar"}]}}
namespace MessagingSystemValues
{
/**
 * Apache ActiveMQ.
 */
// DEBUG: {"brief": "Apache ActiveMQ", "deprecated": none, "id": "activemq", "note": none,
// "stability": "experimental", "value": "activemq"}
static constexpr const char *kActivemq = "activemq";
/**
 * Amazon Simple Queue Service (SQS).
 */
// DEBUG: {"brief": "Amazon Simple Queue Service (SQS)", "deprecated": none, "id": "aws_sqs",
// "note": none, "stability": "experimental", "value": "aws_sqs"}
static constexpr const char *kAwsSqs = "aws_sqs";
/**
 * Azure Event Grid.
 */
// DEBUG: {"brief": "Azure Event Grid", "deprecated": none, "id": "eventgrid", "note": none,
// "stability": "experimental", "value": "eventgrid"}
static constexpr const char *kEventgrid = "eventgrid";
/**
 * Azure Event Hubs.
 */
// DEBUG: {"brief": "Azure Event Hubs", "deprecated": none, "id": "eventhubs", "note": none,
// "stability": "experimental", "value": "eventhubs"}
static constexpr const char *kEventhubs = "eventhubs";
/**
 * Azure Service Bus.
 */
// DEBUG: {"brief": "Azure Service Bus", "deprecated": none, "id": "servicebus", "note": none,
// "stability": "experimental", "value": "servicebus"}
static constexpr const char *kServicebus = "servicebus";
/**
 * Google Cloud Pub/Sub.
 */
// DEBUG: {"brief": "Google Cloud Pub/Sub", "deprecated": none, "id": "gcp_pubsub", "note": none,
// "stability": "experimental", "value": "gcp_pubsub"}
static constexpr const char *kGcpPubsub = "gcp_pubsub";
/**
 * Java Message Service.
 */
// DEBUG: {"brief": "Java Message Service", "deprecated": none, "id": "jms", "note": none,
// "stability": "experimental", "value": "jms"}
static constexpr const char *kJms = "jms";
/**
 * Apache Kafka.
 */
// DEBUG: {"brief": "Apache Kafka", "deprecated": none, "id": "kafka", "note": none, "stability":
// "experimental", "value": "kafka"}
static constexpr const char *kKafka = "kafka";
/**
 * RabbitMQ.
 */
// DEBUG: {"brief": "RabbitMQ", "deprecated": none, "id": "rabbitmq", "note": none, "stability":
// "experimental", "value": "rabbitmq"}
static constexpr const char *kRabbitmq = "rabbitmq";
/**
 * Apache RocketMQ.
 */
// DEBUG: {"brief": "Apache RocketMQ", "deprecated": none, "id": "rocketmq", "note": none,
// "stability": "experimental", "value": "rocketmq"}
static constexpr const char *kRocketmq = "rocketmq";
/**
 * Apache Pulsar.
 */
// DEBUG: {"brief": "Apache Pulsar", "deprecated": none, "id": "pulsar", "note": none, "stability":
// "experimental", "value": "pulsar"}
static constexpr const char *kPulsar = "pulsar";
}  // namespace MessagingSystemValues

}  // namespace messaging
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
