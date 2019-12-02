#pragma once

#include <algorithm>
#include <cstring>
#include <ostream>
#include <string>

namespace opentelemetry {
namespace nostd {
/**
 * Back port of std::string_view to work with pre-cpp-17 compilers.
 *
 * Note: This provides a subset of the methods available on std::string_view but
 * tries to be as compatible as possible with the std::string_view interface.
 */
class string_view {
 public:
  string_view() noexcept : data_(nullptr), length_(0) {}

  string_view(const char* str) noexcept
      : length_(std::strlen(str)), data_(str) {}

  string_view(const std::basic_string<char>& str) noexcept
      : length_(str.length()), data_(str.c_str()) {}

  string_view(const char* str, size_t len) noexcept
      : length_(len), data_(str) {}

  explicit operator std::string() const { return {data_, length_}; }

  const char* data() const noexcept { return data_; }

  bool empty() const noexcept { return length_ == 0; }

  size_t length() const noexcept { return length_; }

  size_t size() const noexcept { return length_; }

  const char* begin() const noexcept { return data(); }

  const char* end() const noexcept { return data() + length(); }

  const char& operator[](std::size_t i) { return *(data() + i); }

 private:
  size_t length_;     // Length of data pointed to by 'data_'
  const char* data_;  // Pointer to external storage
};

inline bool operator==(string_view lhs, string_view rhs) noexcept {
  return lhs.length() == rhs.length() &&
         std::equal(lhs.data(), lhs.data() + lhs.length(), rhs.data());
}

inline bool operator==(string_view lhs, const std::string& rhs) noexcept {
  return lhs == string_view(rhs);
}

inline bool operator==(const std::string& lhs, string_view rhs) noexcept {
  return string_view(lhs) == rhs;
}

inline bool operator==(string_view lhs, const char* rhs) noexcept {
  return lhs == string_view(rhs);
}

inline bool operator==(const char* lhs, string_view rhs) noexcept {
  return string_view(lhs) == rhs;
}

inline bool operator!=(string_view lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(string_view lhs, const std::string& rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(const std::string& lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(string_view lhs, const char* rhs) noexcept {
  return !(lhs == rhs);
}

inline bool operator!=(const char* lhs, string_view rhs) noexcept {
  return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, string_view s) {
  return os.write(s.data(), static_cast<std::streamsize>(s.length()));
}
} // namespace nostd
}  // namespace opentelemetry
