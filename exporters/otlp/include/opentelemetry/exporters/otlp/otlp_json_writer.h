// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Builds one JSON document from a stream of tokens. The OTLP/JSON exporters
 * drive it through ConvertGenericMessageToJson() and send the result of
 * ToString().
 *
 * The interface carries no OTLP knowledge. Mapping decisions, such as
 * hex-encoded trace and span IDs or 64-bit integers written as strings, are
 * made by the caller, which picks the token accordingly.
 *
 * Lifecycle and threading
 *
 * A JsonWriterFactory creates one writer per document. The caller writes the
 * whole document, calls ToString() once, and then destroys the writer. A
 * writer is not reused for a second document, and it is used by one thread
 * at a time. Implementations need no internal synchronization.
 *
 * Token stream
 *
 * - The stream holds exactly one top-level value: a scalar, or a container
 *   opened by BeginObject() or BeginArray().
 * - Every Begin call is matched by the End call of the same kind, and
 *   containers close in the reverse order of opening.
 * - Directly inside an object, each value, including a nested container, is
 *   preceded by exactly one Key(). Directly inside an array, values are not
 *   preceded by Key().
 * - Tokens arrive in output order. A container cannot be extended once a
 *   later sibling has begun.
 * - Callers do not write the same key twice in one object. If they do, the
 *   output is unspecified.
 *
 * Output
 *
 * - A well-formed stream produces a document that is valid JSON, encoded as
 *   UTF-8, with no insignificant whitespace.
 * - Keys and string values escape only what JSON requires: quote, backslash,
 *   backspace, form feed, newline, carriage return and tab as `\"`, `\\`,
 *   `\b`, `\f`, `\n`, `\r` and `\t`, and the other control characters as
 *   `\u00xx` with lowercase hex digits. Every other character is written as
 *   UTF-8. Input
 *   bytes that are not valid UTF-8 are replaced with U+FFFD rather than
 *   treated as an error, so that invalid telemetry data cannot fail an
 *   export.
 * - The order of members in an object is not guaranteed to match the order
 *   of the Key() calls, because JSON objects are unordered. The default
 *   nlohmann-json backend sorts members by key.
 *
 * Errors
 *
 * No method throws, including in builds with exceptions disabled. A
 * malformed token stream or an internal failure, such as a failed
 * allocation, marks the writer as failed instead. ok() returns false from
 * then on, and every later call, including ToString(), is allowed to do
 * nothing. An implementation should report the first failure through the SDK
 * internal log handler.
 */
class JsonWriter
{
public:
  JsonWriter()                              = default;
  JsonWriter(const JsonWriter &)            = delete;
  JsonWriter(JsonWriter &&)                 = delete;
  JsonWriter &operator=(const JsonWriter &) = delete;
  JsonWriter &operator=(JsonWriter &&)      = delete;
  virtual ~JsonWriter()                     = default;

  /**
   * Opens an object. Inside an object, the call is preceded by Key().
   */
  virtual void BeginObject() noexcept = 0;

  /**
   * Closes the innermost open container, which must be an object whose last
   * Key() has received its value.
   */
  virtual void EndObject() noexcept = 0;

  /**
   * Opens an array. Inside an object, the call is preceded by Key().
   */
  virtual void BeginArray() noexcept = 0;

  /**
   * Closes the innermost open container, which must be an array.
   */
  virtual void EndArray() noexcept = 0;

  /**
   * Names the next value of the innermost open container, which must be an
   * object. The key may hold any bytes and is escaped like a string value.
   *
   * @param key valid only for the duration of the call; copy it if it is
   * needed afterwards.
   */
  virtual void Key(nostd::string_view key) noexcept = 0;

  /**
   * Writes the JSON literal null.
   */
  virtual void WriteNull() noexcept = 0;

  /**
   * Writes a JSON string.
   *
   * @param value valid only for the duration of the call; copy it if it is
   * needed afterwards. It may contain any bytes, including NUL.
   */
  virtual void WriteString(nostd::string_view value) noexcept = 0;

  /**
   * Writes a JSON number with the exact value.
   */
  virtual void WriteInt32(std::int32_t value) noexcept = 0;

  /**
   * Writes a JSON number with the exact value over the full 64-bit range. An
   * implementation whose number representation cannot hold every value, such
   * as one that stores all numbers as double, must emit the decimal digits
   * directly.
   */
  virtual void WriteInt64(std::int64_t value) noexcept = 0;

  /**
   * Writes a JSON number with the exact value.
   */
  virtual void WriteUInt32(std::uint32_t value) noexcept = 0;

  /**
   * Writes a JSON number with the exact value over the full 64-bit range. The
   * requirement on WriteInt64() applies.
   */
  virtual void WriteUInt64(std::uint64_t value) noexcept = 0;

  /**
   * Writes a JSON number that parses back to the same double. JSON has no
   * representation for NaN or infinity, so a non-finite value is written as
   * null.
   */
  virtual void WriteDouble(double value) noexcept = 0;

  /**
   * Writes the JSON literal true or false.
   */
  virtual void WriteBool(bool value) noexcept = 0;

  /**
   * Writes the bytes as a JSON string holding their base64 encoding, the same
   * as sdk::common::Base64Escape() produces. Callers that need another
   * encoding, such as hex, encode the bytes themselves and call WriteString().
   *
   * @param data valid only for the duration of the call; may be null when
   * size is 0.
   * @param size the number of bytes at data.
   */
  virtual void WriteBytes(const std::uint8_t *data, std::size_t size) noexcept = 0;

  /**
   * Returns false once the writer has failed. The state is sticky: after it
   * becomes false, it stays false. The call is cheap and valid at any time.
   */
  virtual bool ok() const noexcept = 0;

  /**
   * Returns the serialized document. The caller calls it once, after the
   * top-level value is complete; calling it earlier is a caller error with an
   * unspecified result.
   *
   * Serialization can fail, so the caller checks ok() after the call. When
   * ok() is false, the contents of the returned string are unspecified and
   * must not be used.
   */
  virtual std::string ToString() noexcept = 0;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
