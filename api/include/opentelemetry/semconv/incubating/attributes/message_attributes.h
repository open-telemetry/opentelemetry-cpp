

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
namespace message
{

/**
 * @Deprecated: Replaced by @code rpc.message.compressed_size @endcode.
 */
static const char *kMessageCompressedSize = "message.compressed_size";

/**
 * @Deprecated: Replaced by @code rpc.message.id @endcode.
 */
static const char *kMessageId = "message.id";

/**
 * @Deprecated: Replaced by @code rpc.message.type @endcode.
 */
static const char *kMessageType = "message.type";

/**
 * @Deprecated: Replaced by @code rpc.message.uncompressed_size @endcode.
 */
static const char *kMessageUncompressedSize = "message.uncompressed_size";

// @deprecated(reason="The attribute message.type is deprecated - Replaced by `rpc.message.type`")
// # type: ignore DEBUG: {"brief": "Deprecated, use `rpc.message.type` instead.", "deprecated":
// "Replaced by `rpc.message.type`.", "name": "message.type", "requirement_level": "recommended",
// "root_namespace": "message", "stability": "experimental", "type": {"allow_custom_values": true,
// "members": [{"brief": none, "deprecated": none, "id": "sent", "note": none, "stability":
// "experimental", "value": "SENT"}, {"brief": none, "deprecated": none, "id": "received", "note":
// none, "stability": "experimental", "value": "RECEIVED"}]}}
namespace MessageTypeValues
{
/**
 * sent.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "sent", "note": none, "stability":
// "experimental", "value": "SENT"}
static constexpr const char *kSent = "SENT";
/**
 * received.
 */
// DEBUG: {"brief": none, "deprecated": none, "id": "received", "note": none, "stability":
// "experimental", "value": "RECEIVED"}
static constexpr const char *kReceived = "RECEIVED";
}  // namespace MessageTypeValues

}  // namespace message
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
