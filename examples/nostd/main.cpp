#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include <opentelemetry/nostd/nostd.h>
#include <opentelemetry/version.h>

#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/trace/key_value_iterable_view.h>

#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <array>

using namespace OPENTELEMETRY_NAMESPACE;

/// <summary>
/// Print AttributeValue that contains nostd::span<T>
/// </summary>
/// <typeparam name="T">One of primitive OT protocol types</typeparam>
/// <param name="ss">Output stringstream</param>
/// <param name="value">AttributeValue</param>
/// <param name="jsonTypes">Whether to apply JSON-style formatting</param>
template <typename T>
static void print_array(std::stringstream &ss, common::AttributeValue &value, bool jsonTypes = false)
{
  ss << '[';
  // TODO: do we need to escape string value for JSON?
  auto s    = nostd::get<nostd::span<const T>>(value);
  size_t i  = 1;
  size_t sz = s.size();
  for (auto v : s)
  {
    ss << v; // TODO: nostd::string_view type needs quotes!
    if (i != sz)
      ss << ',';
    i++;
  };
  ss << ']';
};

/// <summary>
/// Utility function to append AttributeValue string representation to stream
/// </summary>
/// <param name="ss">Output stringstream</param>
/// <param name="value">AttributeValue/param>
/// <param name="jsonTypes">Whether to apply JSON-style formatting</param>
static void print_value(std::stringstream &ss,
                        common::AttributeValue &value,
                        bool jsonTypes = false)
{
  switch (value.index())
  {
    case common::AttributeType::TYPE_BOOL:
      if (jsonTypes)
      {
        ss << (nostd::get<bool>(value) ? "true" : "false");
      }
      else
      {
        ss << static_cast<unsigned>(nostd::get<bool>(value));
      }
      break;
    case common::AttributeType::TYPE_INT:
      ss << nostd::get<int32_t>(value);
      break;
    case common::AttributeType::TYPE_INT64:
      ss << nostd::get<int64_t>(value);
      break;
    case common::AttributeType::TYPE_UINT:
      ss << nostd::get<uint32_t>(value);
      break;
    case common::AttributeType::TYPE_UINT64:
      ss << nostd::get<uint64_t>(value);
      break;
    case common::AttributeType::TYPE_DOUBLE:
      ss << nostd::get<double>(value);
      break;
    case common::AttributeType::TYPE_STRING:
      if (jsonTypes)
        ss << '"';
      // TODO: do we need to escape string value for JSON?
      ss << nostd::get<nostd::string_view>(value);
      if (jsonTypes)
        ss << '"';
      break;
    case common::AttributeType::TYPE_CSTRING:
      if (jsonTypes)
        ss << '"';
      // TODO: do we need to escape string value for JSON?
      ss << nostd::get<const char *>(value);
      if (jsonTypes)
        ss << '"';
      break;
#if 0
    case common::AttributeType::TYPE_SPAN_BYTE:
      print_array<uint8_t>(ss, value, jsonTypes);
      break;
#endif
    case common::AttributeType::TYPE_SPAN_BOOL:
      print_array<bool>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_INT:
      print_array<int32_t>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_INT64:
      print_array<int64_t>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_UINT:
      print_array<uint32_t>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_UINT64:
      print_array<uint64_t>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_DOUBLE:
      print_array<double>(ss, value, jsonTypes);
      break;
    case common::AttributeType::TYPE_SPAN_STRING:
      // TODO: print_array doesn't provide the proper quotes
      print_array<nostd::string_view>(ss, value, jsonTypes);
      break;
    default:
      /* TODO: unsupported type - add all other types here for now :) */
      break;
  }
};

/// <summary>
/// Copy KeyValueIterable values into map
/// </summary>
/// <param name="kviv">KeyValueIterable</param>
/// <returns>std::map<std::string, common::AttributeValue></returns>
std::map<std::string, common::AttributeValue> to_map(const trace::KeyValueIterable &kviv)
{
  std::map<std::string, common::AttributeValue> result;
  kviv.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    result[std::string(key.data(), key.size())] = value;
    return true;
  });
  return result;
};

/// <summary>
/// Implementation of API call that accepts KeyValueIterable
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="attributes"></param>
/// <returns></returns
void api_call(const trace::KeyValueIterable &kv) noexcept
{
  std::cout << "## API call KeyValueIterable" << std::endl;
  std::stringstream ss;
  ss << "{";
  size_t size = kv.size();
  if (size)
  {
    size_t i = 1;
    // TODO: we need to do something with this iterator. It is not very convenient.
    // Having range-based for loop would've been nicer
    kv.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
      ss << "\"" << key << "\":";
      print_value(ss, value, true);
      if (size != i)
      {
        ss << ",";
      }
      i++;
      return true;
    });
  };
  ss << "}";
  ss << std::endl;
  std::cout << ss.str();
}

/// <summary>
/// Implementation of API call that accepts a span of AttributeValue
/// </summary>
/// <param name="values"></param>
/// <returns></returns>
void api_call_span(const nostd::span<common::AttributeValue> values) noexcept
{
  std::cout << "## API call using nostd::span" << std::endl;
  std::stringstream ss;
  size_t i = 0;
  ss << "[";
  for (auto &&v : values)
  {
    if (i)
      ss << ',';
    print_value(ss, v, true);
    i++;
  }
  ss << "]";
  ss << std::endl;
  std::cout << ss.str();
};

/// <summary>
/// Example showing how to pass a map of attributes to OT API call.
/// </summary>
void test_api_map()
{
  std::array<uint64_t, 5> a1{{1, 2, 3, 4, 5}};
  std::map<nostd::string_view, common::AttributeValue> m1 = {
      {"uint64key", (uint64_t)1},
      {"stringKey", nostd::string_view("Hello")},
      {"boolKey", false},
      {"arrayUint64", nostd::span<uint64_t, 5>{a1}}};
  std::cout << "# Testing std::map..." << std::endl;
  api_call(trace::KeyValueIterableView<decltype(m1)>{m1});

  // This code requires need to provide std::hash<nostd::string_view>
  std::unordered_map<nostd::string_view, common::AttributeValue> m2 = {
      {"uint64key", (uint64_t)1},
      {"stringKey", nostd::string_view("Hello")},
      {"boolKey", false},
      {"arrayUint64", nostd::span<uint64_t, 5>{a1}}};
  std::cout << "# Testing std::unordered_map..." << std::endl;
  api_call(trace::KeyValueIterableView<decltype(m2)>{m2});
}

/// <summary>
/// Example showing how to pass a vector of values to OT API call as span.
/// </summary>
void test_api_vector()
{
  std::vector<common::AttributeValue> v{12345, "Hello"};
  api_call_span(nostd::span<common::AttributeValue>{v.data(), v.size()});
}

int main(int argc, char *argv[])
{
  test_api_map();
  test_api_vector();
  return 0;
}
