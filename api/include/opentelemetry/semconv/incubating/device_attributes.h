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
namespace device
{

/**
 * A unique identifier representing the device
 * <p>
 * The device identifier MUST only be defined using the values outlined below. This value is not an
 * advertising identifier and MUST NOT be used as such. On iOS (Swift or Objective-C), this value
 * MUST be equal to the <a
 * href="https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor">vendor
 * identifier</a>. On Android (Java or Kotlin), this value MUST be equal to the Firebase
 * Installation ID or a globally unique UUID which is persisted across sessions in your application.
 * More information can be found <a
 * href="https://developer.android.com/training/articles/user-data-ids">here</a> on best practices
 * and exact implementation details. Caution should be taken when storing personal data or anything
 * which can identify a user. GDPR and data protection laws may apply, ensure you do your own due
 * diligence.
 */
static constexpr const char *kDeviceId = "device.id";

/**
 * The name of the device manufacturer
 * <p>
 * The Android OS provides this field via <a
 * href="https://developer.android.com/reference/android/os/Build#MANUFACTURER">Build</a>. iOS apps
 * SHOULD hardcode the value @code Apple @endcode.
 */
static constexpr const char *kDeviceManufacturer = "device.manufacturer";

/**
 * The model identifier for the device
 * <p>
 * It's recommended this value represents a machine-readable version of the model identifier rather
 * than the market or consumer-friendly name of the device.
 */
static constexpr const char *kDeviceModelIdentifier = "device.model.identifier";

/**
 * The marketing name for the device model
 * <p>
 * It's recommended this value represents a human-readable version of the device model rather than a
 * machine-readable alternative.
 */
static constexpr const char *kDeviceModelName = "device.model.name";

}  // namespace device
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
