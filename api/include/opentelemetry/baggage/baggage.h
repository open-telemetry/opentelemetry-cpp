// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cctype>

#include "opentelemetry/baggage/baggage_impl.h"
#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace baggage
{

class Baggage
{
public:
  Baggage() : baggage_impl_(new BaggageImpl()) {}
  Baggage(size_t size) : baggage_impl_(new BaggageImpl(size)){};

  // allow Baggage to be copied
  Baggage(const Baggage &) = default;
  Baggage &operator=(const Baggage &) = default;

  template <class T>
  Baggage(const T &keys_and_values) : baggage_impl_(keys_and_values)
  {}

  static Baggage GetDefault() { return Baggage(); }

  /* Get value for key in the baggage
     @returns true if key is found, false otherwise
  */
  bool GetValue(nostd::string_view key, std::string &value) const
  {
    return baggage_impl_->GetValue(key, value);
  }

  /* Returns shared_ptr of new baggage object which contains new key-value pair. If key or value is
     invalid, copy of current baggage is returned
  */
  Baggage Set(const nostd::string_view &key, const nostd::string_view &value)
  {
    return Baggage(baggage_impl_->Set(key, value));
  }

  // @return all key-values entries by repeatedly invoking the function reference passed as argument
  // for each entry
  bool GetAllEntries(
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)> callback) const noexcept
  {
    return baggage_impl_->GetAllEntries(callback);
  }

  // delete key from the baggage if it exists. Returns shared_ptr of new baggage object.
  // if key does not exist, copy of current baggage is returned.
  // Validity of key is not checked as invalid keys should never be populated in baggage in the
  // first place.
  Baggage Delete(nostd::string_view key) { return Baggage(baggage_impl_->Delete(key)); }

  // Returns shared_ptr of baggage after extracting key-value pairs from header
  static Baggage FromHeader(nostd::string_view header)
  {
    return Baggage(BaggageImpl::FromHeader(header));
  }

  // Creates string from baggage object.
  std::string ToHeader() const { return baggage_impl_->ToHeader(); }

private:
  Baggage(BaggageImpl *baggage_impl) : baggage_impl_(baggage_impl) {}
  nostd::shared_ptr<BaggageImpl> baggage_impl_;
};

}  // namespace baggage

OPENTELEMETRY_END_NAMESPACE
