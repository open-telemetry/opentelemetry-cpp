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
namespace kafka
{

/**
  The Kafka cluster ID, obtained from the broker metadata exposed through the Kafka client (or
  AdminClient) API. <p> The cluster ID is a unique identifier reported by the Kafka broker. It
  identifies the cluster independently of the individual brokers the client is configured to connect
  to, and remains stable even if broker hostnames, IP addresses, or ports change.
 */
static constexpr const char *kKafkaClusterId = "kafka.cluster.id";

/**
  Message keys in Kafka are used for grouping alike messages to ensure they're processed on the same
  partition. They differ from @code messaging.message.id @endcode in that they're not unique. If the
  key is @code null @endcode, the attribute MUST NOT be set. <p> If the key type is not string, its
  string representation has to be supplied for the attribute. If the key has no unambiguous,
  canonical string form, don't include its value.
 */
static constexpr const char *kKafkaMessageKey = "kafka.message.key";

/**
  A boolean that is true if the message is a tombstone.
 */
static constexpr const char *kKafkaMessageTombstone = "kafka.message.tombstone";

/**
  The offset of a record in the corresponding Kafka partition.
 */
static constexpr const char *kKafkaOffset = "kafka.offset";

}  // namespace kafka
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
