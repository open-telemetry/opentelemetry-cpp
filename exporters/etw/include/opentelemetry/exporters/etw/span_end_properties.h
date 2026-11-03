// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
{
class Tracer;
namespace detail
{
class SpanEndProperties
{
  using Entry = std::pair<std::string, PropertyValue>;

public:
  static constexpr size_t kCapacity = 11;

  class Iterator
  {
  public:
    Iterator(const SpanEndProperties &owner, size_t index) : owner_(owner), index_(index) {}
    Iterator(const SpanEndProperties &owner, PropertyValueMap::const_iterator position)
        : owner_(owner), position_(position)
    {}

    PropertyReference operator*() const
    {
      if (owner_.overflow_)
      {
        return {position_->first, position_->second};
      }
      const auto &entry = owner_.entries_[index_];
      return {entry.first, entry.second};
    }

    Iterator &operator++()
    {
      if (owner_.overflow_)
      {
        ++position_;
      }
      else
      {
        ++index_;
      }
      return *this;
    }

    bool operator!=(const Iterator &other) const
    {
      return owner_.overflow_ ? position_ != other.position_ : index_ != other.index_;
    }

  private:
    const SpanEndProperties &owner_;
    size_t index_ = 0;
    PropertyValueMap::const_iterator position_{};
  };

  PropertyValue &operator[](const std::string &key)
  {
    if (overflow_)
    {
      return (*overflow_)[key];
    }
    for (size_t i = 0; i < count_; ++i)
    {
      if (nostd::string_view(entries_[i].first) == nostd::string_view(key))
      {
        return entries_[i].second;
      }
    }
    if (count_ == kCapacity)
    {
      // Preserve every entry before switching; never truncate in release builds.
      auto owned = std::unique_ptr<Properties>(new Properties);
      for (size_t i = 0; i < count_; ++i)
      {
        (*owned)[entries_[i].first] = entries_[i].second;
      }
      overflow_ = std::move(owned);
      for (size_t i = 0; i < count_; ++i)
      {
        entries_[i] = Entry{};
      }
      count_ = 0;
      return (*overflow_)[key];
    }
    // Own keys too: custom field names need not be literals or long-lived strings.
    entries_[count_].first = key;
    return entries_[count_++].second;
  }

  void Sort()
  {
    if (!overflow_)
    {
      std::sort(entries_.begin(), entries_.begin() + count_,
                [](const Entry &left, const Entry &right) {
                  return nostd::string_view(left.first) < nostd::string_view(right.first);
                });
    }
  }

  Iterator begin() const
  {
    return overflow_ ? Iterator(*this, overflow_->begin()) : Iterator(*this, size_t{0});
  }
  Iterator end() const
  {
    return overflow_ ? Iterator(*this, overflow_->end()) : Iterator(*this, count_);
  }
  Iterator find(const std::string &key) const
  {
    if (overflow_)
    {
      return Iterator(*this, overflow_->find(key));
    }
    for (size_t i = 0; i < count_; ++i)
    {
      if (nostd::string_view(entries_[i].first) == nostd::string_view(key))
      {
        return Iterator(*this, i);
      }
    }
    return end();
  }
  bool empty() const { return size() == 0; }
  size_t size() const { return overflow_ ? overflow_->size() : count_; }

private:
  std::array<Entry, kCapacity> entries_;
  size_t count_ = 0;
  std::unique_ptr<Properties> overflow_;
};
}  // namespace detail
}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
