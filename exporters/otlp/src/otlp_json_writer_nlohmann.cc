// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_json_writer_factory_nlohmann.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "opentelemetry/exporters/otlp/otlp_json_writer.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/base64.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{

class NlohmannJsonWriter final : public JsonWriter
{
public:
  void BeginObject() noexcept override
  {
    OpenContainer(nlohmann::json::object(), Container::kObject);
  }

  void EndObject() noexcept override { CloseContainer(Container::kObject); }

  void BeginArray() noexcept override { OpenContainer(nlohmann::json::array(), Container::kArray); }

  void EndArray() noexcept override { CloseContainer(Container::kArray); }

  void Key(nostd::string_view key) noexcept override
  {
    if (!ok_)
    {
      return;
    }
    if (stack_.empty() || stack_.back().kind != Container::kObject)
    {
      Fail("Key() called outside an object");
      return;
    }
    if (has_pending_key_)
    {
      Fail("Key() called twice without an intervening value");
      return;
    }
    pending_key_     = std::string(key.data(), key.size());
    has_pending_key_ = true;
  }

  void WriteNull() noexcept override { ClaimSlot(); }

  void WriteString(nostd::string_view value) noexcept override
  {
    if (nlohmann::json *slot = ClaimSlot())
    {
      *slot = std::string(value.data(), value.size());
    }
  }

  void WriteInt32(std::int32_t value) noexcept override { WriteScalar(value); }

  void WriteInt64(std::int64_t value) noexcept override { WriteScalar(value); }

  void WriteUInt32(std::uint32_t value) noexcept override { WriteScalar(value); }

  void WriteUInt64(std::uint64_t value) noexcept override { WriteScalar(value); }

  void WriteDouble(double value) noexcept override { WriteScalar(value); }

  void WriteBool(bool value) noexcept override { WriteScalar(value); }

  void WriteBytes(const std::uint8_t *data, std::size_t size) noexcept override
  {
    if (nlohmann::json *slot = ClaimSlot())
    {
      *slot = opentelemetry::sdk::common::Base64Escape(
          std::string(reinterpret_cast<const char *>(data), size));
    }
  }

  bool ok() const noexcept override { return ok_; }

  std::string ToString() noexcept override
  {
    return root_.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
  }

private:
  enum class Container
  {
    kObject,
    kArray,
  };

  struct Frame
  {
    nlohmann::json *node;
    Container kind;
  };

  template <typename T>
  void WriteScalar(T value) noexcept
  {
    if (nlohmann::json *slot = ClaimSlot())
    {
      *slot = value;
    }
  }

  // Returns the slot the next value is written into, consuming the pending
  // key or appending an array element. Fails and returns nullptr if the token
  // stream is malformed.
  nlohmann::json *ClaimSlot() noexcept
  {
    if (!ok_)
    {
      return nullptr;
    }

    if (stack_.empty())
    {
      if (root_set_)
      {
        Fail("a value was written after the top-level value was already complete");
        return nullptr;
      }
      root_set_ = true;
      return &root_;
    }

    Frame &frame = stack_.back();
    if (frame.kind == Container::kArray)
    {
      frame.node->push_back(nlohmann::json());
      return &frame.node->back();
    }

    // kObject
    if (!has_pending_key_)
    {
      Fail("a value was written without a preceding Key()");
      return nullptr;
    }
    has_pending_key_     = false;
    nlohmann::json &slot = (*frame.node)[pending_key_];
    return &slot;
  }

  void OpenContainer(nlohmann::json &&empty_container, Container kind) noexcept
  {
    nlohmann::json *slot = ClaimSlot();
    if (!slot)
    {
      return;
    }
    *slot = std::move(empty_container);
    stack_.push_back(Frame{slot, kind});
  }

  void CloseContainer(Container kind) noexcept
  {
    if (!ok_)
    {
      return;
    }
    if (stack_.empty() || stack_.back().kind != kind)
    {
      Fail("mismatched Begin/End call");
      return;
    }
    if (stack_.back().kind == Container::kObject && has_pending_key_)
    {
      Fail("object closed with a Key() that has no value");
      return;
    }
    stack_.pop_back();
  }

  void Fail(const char *reason) noexcept
  {
    if (!ok_)
    {
      return;
    }
    ok_ = false;
    OTEL_INTERNAL_LOG_ERROR("[OTLP JSON Writer] " << reason);
  }

  nlohmann::json root_;
  std::vector<Frame> stack_;
  std::string pending_key_;
  bool has_pending_key_ = false;
  bool root_set_        = false;
  bool ok_              = true;
};

}  // namespace

std::unique_ptr<JsonWriter> JsonWriterFactoryNlohmann::Create()
{
  return std::make_unique<NlohmannJsonWriter>();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
