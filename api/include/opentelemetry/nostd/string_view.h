#pragma once

#include <algorithm>
#include <cstring>
#include <ostream>
#include <string>
#include <stdexcept>

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
  static constexpr std::size_t npos = std::string::npos;

  string_view() noexcept : length_(0), data_(nullptr) {}

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

  string_view substr(std::size_t pos, std::size_t n = npos) const {
    if (pos > length_) {
      throw std::out_of_range{"opentelemetry::nostd::string_view"};
    }
    n = (std::min)(n, length_ - pos);
    return string_view(data_ + pos, n);
  }

 private:
  // Note: uses the same binary layout as libstdc++'s std::string_view
  // See https://github.com/gcc-mirror/gcc/blob/e0c554e4da7310df83bb1dcc7b8e6c4c9c5a2a4f/libstdc%2B%2B-v3/include/std/string_view#L466-L467
  size_t length_;
  const char* data_;
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
