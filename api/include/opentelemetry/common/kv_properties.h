#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

// Class to store fixed size array of key-value pairs of string type
class KeyValueProperties
{
  // Class to store key-value pairs of string types
public:
  class Entry
  {
  public:
    Entry() : key_(nullptr), value_(nullptr){};

    // Copy constructor
    Entry(const Entry &copy)
    {
      key_   = CopyStringToPointer(copy.key_.get());
      value_ = CopyStringToPointer(copy.value_.get());
    }

    // Copy assignment operator
    Entry &operator=(Entry &other)
    {
      key_   = CopyStringToPointer(other.key_.get());
      value_ = CopyStringToPointer(other.value_.get());
      return *this;
    }

    // Move contructor and assignment operator
    Entry(Entry &&other) = default;
    Entry &operator=(Entry &&other) = default;

    // Creates an Entry for a given key-value pair.
    Entry(nostd::string_view key, nostd::string_view value) noexcept
    {
      key_   = CopyStringToPointer(key);
      value_ = CopyStringToPointer(value);
    }

    // Gets the key associated with this entry.
    nostd::string_view GetKey() const { return key_.get(); }

    // Gets the value associated with this entry.
    nostd::string_view GetValue() const { return value_.get(); }

    // Sets the value for this entry. This overrides the previous value.
    void SetValue(nostd::string_view value) { value_ = CopyStringToPointer(value); }

  private:
    // Store key and value as raw char pointers to avoid using std::string.
    nostd::unique_ptr<const char[]> key_;
    nostd::unique_ptr<const char[]> value_;

    // Copies string into a buffer and returns a unique_ptr to the buffer.
    // This is a workaround for the fact that memcpy doesn't accept a const destination.
    nostd::unique_ptr<const char[]> CopyStringToPointer(nostd::string_view str)
    {
      char *temp = new char[str.size() + 1];
      memcpy(temp, str.data(), str.size());
      temp[str.size()] = '\0';
      return nostd::unique_ptr<const char[]>(temp);
    }
  };

  // Maintain the number of entries in entries_.
  size_t num_entries_;

  // Max size of allocated array
  size_t max_num_entries_;
  // Store entries in a C-style array to avoid using std::array or std::vector.
  nostd::unique_ptr<Entry[]> entries_;

public:
  // Create Key-value list of given size
  // @param size : Size of list.
  KeyValueProperties(size_t size)
      : num_entries_(0), max_num_entries_(size), entries_(new Entry[size])
  {}

  // Create Empty Key-Value list
  KeyValueProperties() : num_entries_(0), max_num_entries_(0), entries_(nullptr) {}

  /**
   * TBD - Check if Key-Value iterable, and has size() method
   */

  template <class T>
  KeyValueProperties(const T &keys_and_values)
      : max_num_entries_(keys_and_values.size()), entries_(new Entry[max_num_entries_])
  {
    size_t index = 0;
    for (auto &e : keys_and_values)
    {
      Entry entry(keys_and_values.first, keys_and_values.second);
      (entries_.get())[num_entries_++] = entry;
    }
  }

  void AddEntry(const nostd::string_view &key, const nostd::string_view &value)
  {
    if (num_entries_ < max_num_entries_)
    {
      Entry entry(key, value);
      (entries_.get())[num_entries_++] = entry;
    }
  }

  bool GetAllEntries(
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)> callback) const noexcept
  {
    for (size_t i = 0; i < num_entries_; i++)
    {
      auto entry = (entries_.get())[i];
      if (!callback(entry.GetKey(), entry.GetValue()))
      {
        return false;
      }
    }
    return true;
  }

  bool GetValue(const nostd::string_view key, std::string &value)
  {
    for (size_t i = 0; i < num_entries_; i++)
    {
      auto entry = (entries_.get())[i];
      if (entry.GetKey() == key)
      {
        value = std::string(entry.GetValue());
        return true;
      }
    }
    return false;
  }

  size_t Size() { return num_entries_; }
};
}  // namespace common
OPENTELEMETRY_END_NAMESPACE
