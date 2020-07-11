#pragma once
#include "opentelemetry/nostd/string_view.h"
#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
template<class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
public abstract class TraceState {
  private:
    static const int kKeyMaxSize = 256;
    static const int kValueMaxSize = 256;
    static const int kMaxKeyValuePairs = 32;
    static const TraceState kDefault = TraceState.builder().build();

    KeyValueIterable entries_;
    TraceState(KeyValueIterable entries) = default; // I am not sure how default works, but just from observation of other classes

    TraceState(T &entries) {
        return TraceState(KeyValueIterableView<T>(entries));
    }

    virtual static TraceState Create(KeyValueIterable entries) = 0;

    static TraceState Create(T &entries) {
        return Create(KeyValueIterableView<T>(entries));
    }

// To Reviewer: deleted after switching to KeyValueIterable
//    static TraceState Create(nostd::span<Entry> entries) {
//      return TraceState(entries);
//    }
//    TraceState(Entry[] entries) {
//        entries_ = entries;
//    }
//    Entry entries_[kMaxKeyValuePairs];
//    int size;

  public:
    // Builder class for TraceState.
    static const class Builder {
      private:
        const TraceState parent_;
        KeyValueIterable entries_;
        const KeyValueIterable kEmptyEntries;
//        Entry entries_[kMaxKeyValuePairs];
//        const Entry kEmptyEntries[kMaxKeyValuePairs];
        // Needs to be in this class to avoid initialization deadlock because super class depends on
        // subclass (the auto-value generate class).
        static const TraceState kEmpty = Create(kEmptyEntries);
//        int size = 0;

        Builder(TraceState &parent) {
          parent_ = parent;
        }

      public:
        // Adds or updates the Entry that has the given key if it is present. The new
        // Entry will always be added in the front of the list of entries.
        Builder Set(nostd::string_view &key, nostd::string_view &value) {
          // Initially create the Entry to validate input.
//          Entry entry = Entry.Create(key, value);
          if (size == 0) {
            // Copy entries from the parent.
            entries_ = parent_.GetEntries();
//            for (Entry entry : parent.GetEntries()) {
//                entries_[size] = entry;
//                size++;
//            }
          }
          entries_.ForEachKeyValue([&](nostd::string_view k, nostd::string_view &v) {
            if (k == key) v = value; // Is this what we should do to use KeyValueIterable here?
          });
//          int pos = -1;
//          for (int i = 0; i < size; i++) {
//            if (entries[i].GetKey() == entry.GetKey()) {
//              size--;
//              pos = i;
//              // Exit now because the entries list cannot contain duplicates.
//              break;
//            }
//          }
//          size++;
          // Inserts the element at the front of this list.
//          if (pos != -1) entries_[pos] = entry;
//          else entries_[size - 1] = entry;
          return this;
        }

        // Removes the Entry that has the given key if it is present.
        Builder Remove(nostd::string_view &key) {
          if (size == 0) {
            // Copy entries from the parent.
            entries_ = parent_.GetEntries();
//            for (Entry entry : parent.GetEntries()) {
//              entries_[size] = entry;
//              size++;
//            }
          }
          entries_.ForEachKeyValue([&](nostd::string_view k, nostd::string_view &v) {
            if (k == key) v = value; // How do we remove a key-val pair here?
          });
//          for (int i = 0; i < size(); i++) {
//            if (entries[i] == NULL) continue;
//            if (entries[i].GetKey() == key) {
//              entries[i] = NULL;
//              size--;
//              // Exit now because the entries list cannot contain duplicates.
//              break;
//            }
//          }
          return this;
        }

        // Builds a TraceState by adding the entries to the parent in front of the key-value pairs list
        // and removing duplicate entries.
        TraceState Build() {
          if (entries == NULL) {
            return parent;
          }
          return TraceState.Create(entries);
        }
    }
// To Reviewer: Removed because no longer needed after switching to KeyValueIterable
//    // Immutable key-value pair for TraceState.
//    static class Entry {
//      // Creates a new Entry for the TraceState.
//      public:
//        static Entry Create(nostd::string_view &key, nostd::string_view &value) {
//          // Again what is this again I am not sure
//          return Entry();
//        }
//
//        // Returns the key.
//        virtual nostd::string_view GetKey() = 0;
//
//        // Returns the value.
//        virtual nostd::string_view GetValue() = 0;
//
//        Entry() {}
//      private:
//        const nostd::string_view key_;
//        const nostd::string_view value_;
//    }

    // Returns the default TraceState with no entries.
    static TraceState GetDefault() {
      return kDefault;
    }

    // Returns the value to which the specified key is mapped, or null if this map contains no mapping
    // for the key.
    nostd::string_view Get(nostd::string_view key) {
      GetEntries().ForEachKeyValue([&key](nostd::string_view k, nostd::v){
        if (key == k) return v;
      });
//  For reviewer: Below is the original code I had, which is directly translated from the Java implementation
//      for (Entry entry : GetEntries()) {
//        if (entry.GetKey() == key)) {
//          return entry.GetValue();
//        }
//      }
      return NULL;
    }

    // Returns a list view of the mappings contained in this TraceState.
    virtual KeyValueIterable GetEntries() = 0;

    // Returns a Builder based on an empty TraceState.
    static Builder Builder() {
      return Builder(Builder.kEmpty);
    }

    TraceState() {}
}
}
OPENTELEMETRY_END_NAMESPACE