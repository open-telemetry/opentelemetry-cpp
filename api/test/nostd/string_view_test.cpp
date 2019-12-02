#include "opentelemetry/nostd/string_view.h"

#include "opentelemetry/3rd_party/catch.hpp"

using opentelemetry::nostd::string_view;

TEST_CASE("string_view provides a back port of std::string_view") {
  SECTION("A default-constructed string_view is empty.") {
    string_view ref;
    REQUIRE(nullptr == ref.data());
    REQUIRE(0 == ref.length());
  }

  SECTION("string_view can be initialized from a c-string.") {
    const char* val = "hello world";

    string_view ref(val);

    REQUIRE(val == ref.data());
    REQUIRE(std::strlen(val) == ref.length());
  }

  SECTION("string_view can be initialized from an std::string.") {
    const std::string val = "hello world";

    string_view ref(val);

    REQUIRE(val == ref.data());
    REQUIRE(val.length() == ref.length());
  }

  SECTION("A copied string_view points to the same data as its source.") {
    const std::string val = "hello world";

    string_view ref(val);
    string_view cpy(ref);

    REQUIRE(val == cpy.data());
    REQUIRE(val.length() == cpy.length());
  }

  SECTION("operator[] can be used to access characters in a string_view") {
    string_view s = "abc123";
    REQUIRE(&s[0] == s.data());
    REQUIRE(&s[1] == s.data() + 1);
  }

  SECTION("a string_view can be explicitly converted to an std::string") {
    std::string s = static_cast<std::string>(string_view{"abc"});
    REQUIRE(s == "abc");
  }
}
