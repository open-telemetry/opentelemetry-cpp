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
namespace rocketmq
{

/**
  Model of message consumption. This only applies to consumer spans.
 */
static constexpr const char *kRocketmqConsumptionModel = "rocketmq.consumption_model";

/**
  The delay time level for delay message, which determines the message delay time.
 */
static constexpr const char *kRocketmqMessageDelayTimeLevel = "rocketmq.message.delay_time_level";

/**
  The timestamp in milliseconds that the delay message is expected to be delivered to consumer.
 */
static constexpr const char *kRocketmqMessageDeliveryTimestamp =
    "rocketmq.message.delivery_timestamp";

/**
  It is essential for FIFO message. Messages that belong to the same message group are always
  processed one by one within the same consumer group.
 */
static constexpr const char *kRocketmqMessageGroup = "rocketmq.message.group";

/**
  Key(s) of message, another way to mark message besides message ID.
 */
static constexpr const char *kRocketmqMessageKeys = "rocketmq.message.keys";

/**
  The secondary classifier of message besides topic.
 */
static constexpr const char *kRocketmqMessageTag = "rocketmq.message.tag";

/**
  Type of message.
 */
static constexpr const char *kRocketmqMessageType = "rocketmq.message.type";

/**
  Namespace of RocketMQ resources, resources in different namespaces are individual.
 */
static constexpr const char *kRocketmqNamespace = "rocketmq.namespace";

namespace RocketmqConsumptionModelValues
{
/**
  Clustering consumption model
 */
static constexpr const char *kClustering = "clustering";

/**
  Broadcasting consumption model
 */
static constexpr const char *kBroadcasting = "broadcasting";

}  // namespace RocketmqConsumptionModelValues

namespace RocketmqMessageTypeValues
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

}  // namespace RocketmqMessageTypeValues

}  // namespace rocketmq
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
