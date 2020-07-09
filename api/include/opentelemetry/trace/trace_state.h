#pragma once
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
public abstract class TraceState {
  private:
    static const int KEY_MAX_SIZE = 256;
    static const int VALUE_MAX_SIZE = 256;
    static const int MAX_KEY_VALUE_PAIRS = 32;
    static const TraceState DEFAULT = TraceState.builder().build();

    static TraceState create(nostd::span<Entry> entries) {
      return TraceState(entries);
    }
    TraceState(nostd::span<Entry> entries) {
        this.entries = entries;
    }
    nostd::span<Entry> entries;

  public:
    // Builder class for TraceState.
    static const class Builder {
      private:
        const TraceState parent;
        nostd::span<Entry> entries;
        const nostd::span<Entry> EMPTY_ENTRIES;
        // Needs to be in this class to avoid initialization deadlock because super class depends on
        // subclass (the auto-value generate class).
        static const TraceState EMPTY = create(EMPTY_ENTRIES);

        Builder(TraceState &parent) {
          this.parent = parent;
          this.entries = NULL;
        }

      public:
        // Adds or updates the Entry that has the given key if it is present. The new
        // Entry will always be added in the front of the list of entries.
        Builder set(nostd::string_view &key, nostd::string_view &value) {
          // Initially create the Entry to validate input.
          Entry entry = Entry.create(key, value);
          if (entries == NULL) {
            // Copy entries from the parent.
            entries = parent.getEntries();
          }
          for (int i = 0; i < entries.size(); i++) {
            if (entries[i].getKey() == entry.getKey()) {
              entries.erase(entries.begin()+i);
              // Exit now because the entries list cannot contain duplicates.
              break;
            }
          }
          // Inserts the element at the front of this list.
          entries.insert(entries.begin(), entry);
          return this;
        }

        // Removes the Entry that has the given key if it is present.
        Builder remove(nostd::string_view &key) {
          if (entries == NULL) {
            // Copy entries from the parent.
            entries = parent.getEntries();
          }
          for (int i = 0; i < entries.size(); i++) {
            if (entries[i].getKey() == key) {
              entries.erase(entries.begin()+i);
              // Exit now because the entries list cannot contain duplicates.
              break;
            }
          }
          return this;
        }

        // Builds a TraceState by adding the entries to the parent in front of the key-value pairs list
        // and removing duplicate entries.
        TraceState build() {
          if (entries == NULL) {
            return parent;
          }
          return TraceState.create(entries);
        }
    }

    // Immutable key-value pair for TraceState.
    static class Entry {
      // Creates a new Entry for the TraceState.
      public:
        static Entry create(nostd::string_view &key, nostd::string_view &value) {
          // Again what is this again I am not sure
          return new AutoValue_TraceState_Entry(key, value);
        }

        // Returns the key.
        virtual nostd::string_view getKey() = 0;

        // Returns the value.
        virtual nostd::string_view getValue() = 0;

        Entry() {}
    }

    // Returns the default TraceState with no entries.
    static TraceState getDefault() {
      return DEFAULT;
    }

    // Returns the value to which the specified key is mapped, or null if this map contains no mapping
    // for the key.
    nostd::string_view get(nostd::string_view key) {
      for (Entry entry : getEntries()) {
        if (entry.getKey() == key)) {
          return entry.getValue();
        }
      }
      return NULL;
    }

    // Returns a list view of the mappings contained in this TraceState.
    virtual nostd::span<Entry> getEntries() = 0;

    // Returns a Builder based on an empty TraceState.
    static Builder builder() {
      return Builder(Builder.EMPTY);
    }

    TraceState() {}
}
}
OPENTELEMETRY_END_NAMESPACE