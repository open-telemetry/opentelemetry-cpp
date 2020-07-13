# Usage example for nostd classes

This example illustrates the most common usage patterns for nostd classes.

| Type | Scenario |
| -- | -- |
| ```nostd::span<common::AttributeValue>``` | Passing a contiguous region of variant objects to API |
| ```trace::KeyValueIterableView``` | Passing key-value pairs collection to API |
| ```nostd::string_view``` | Can be used as key in std::map and unordered_map |
| ```common::AttributeValue``` | Can be used to express any strong type supported by OT protocol |
| ```nostd::variant``` | Backport of ```std::variant``` to C++11 and above compiler |

Example that is transforming input data from standard types to ABI-stable types:

```cpp

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

```

Example output:

```
# Testing std::map...
## API call KeyValueIterable
{"arrayUint64":[1,2,3,4,5],"boolKey":false,"stringKey":"Hello","uint64key":1}
# Testing std::unordered_map...
## API call KeyValueIterable
{"uint64key":1,"stringKey":"Hello","boolKey":false,"arrayUint64":[1,2,3,4,5]}
## API call using nostd::span
[12345,"Hello"]
```
