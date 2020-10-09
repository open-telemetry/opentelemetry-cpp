# Event Attributes and Property List flattening

This document illustrates the mapping from 'flat' Event property lists to JavaScript Object Notation (JSON) Pointer.

Flat map of JSON Pointer notation key-value pairs can be unflattened into JSON object.

See [RFC6901](https://tools.ietf.org/html/rfc6901) for more details.

## Supported property list formats at SDK API layer

Supported property list input formats are:

- Event key-value pairs with custom prefix, e.g. **custom_** or **_** prefix for property names.

- **JSONPath** in dotted notation - no special prefix property names, e.g. **ext.device.id** 

All of these notations could be converted to RFC6901 first. Then unflattened into JSON object:

```
+---------------+    +--------------+    +-------------+
| Property List | => | RFC6091 List | => | JSON object |
+---------------+    +--------------+    +-------------+
```

## Reference implementation

This document provides generic guidelines applicable to any programming language.

Reference implementation is provided in C++.

Code below:

- reference implementation of the transformations in C++.
- uses STL ```map<string, string>``` for illustrative purposes. Real-world SDK will operate on ```EventProperties``` map or ```map<string, variant<...>>```
- implementation for other languages is not provided, but should no exceed ~10 lines of code.
- [nlohmann/json](https://github.com/nlohmann/json) library (already used by OpenTelemetry C++ SDK in zPages module)

Several existing implementations of of JSON Pointer can be found at [JSON Patch specification page](http://jsonpatch.com/), which also uses JSON Pointer to express individual JSON tree nodes paths. [Asp.Net Core JsonPatch](https://github.com/aspnet/JsonPatch) is Microsoft official implementation for C#.

## Background

Assuming that SDK provides several options of populating event contents:

- ```SetProperty(string key, variant value)```
- initializer lists - "fluent", inline, almost JSON-style initialization for C++

SetProperty usage example:

```cpp
    EventProperties evt(eventName);
    evt.SetProperty(FIELD_A_KEY, FIELD_A_VALUE);
    evt.SetProperty(FIELD_B_KEY, FIELD_B_VALUE);
```

Initializer list usage example:

```cpp
   EventProperties evt(eventName,
       {
           { "strKey1",  "hello1" },
           { "strKey2",  "hello2" },
           { "int64Key", int64_t(1L) },
           { "dblKey",   3.14 },
           { "boolKey",  false },
           { "guidKey0", GUID_t("00000000-0000-0000-0000-000000000000") },
           { "guidKey1", GUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F") },
           { "guidKey2", GUID_t("00010203-0405-0607-0809-0A0B0C0D0E0F") },
       });
```

## Expressing deep nested structures

Example:

```cpp
   EventProperties evt(eventName,
       {
           { "ext.device.id",       "c:12345678" },
           { "ext.app.name",        "c:myCustomAppName" }
           { "ext.metadata.fooBar", 12345 }
       });
```

Note that in this example we used JSONPath dotted notation as property name.

Other notable examples of property names:

| Property | Notation |
|--|--|
|ext.foo.bar|JSONPath|
|/ext/foo/bar| JSON Pointer (rfc6901)|
|PartA_ext_foo_bar|ETW event property name (does not support dots in field names) |

All of these notations can be converted to JSON Pointer path (rfc6901) in a lossless, one-to-one mapping manner.

Subsequently JSON Pointer path notation can be unflattened into JSON object by Telemetry Agent or Protocol Exporter.

It may be necessary on certain paths to Telemetry Agent (e.g. ETW) to utilize `underscores` instead of `dots`, in formats where dots are not allowed. Using underscores instead of dots may be necessary for several backend flows that do not support fields with dots. For example, not every SQL database accepts dots.

SDK API surface accepts any notation,  automagically translates into corresponding notation supported by lower-level transport at exporter layer.

Separate implementation will be provided to transform to CsRecord on Bond (direct upload) path.

## Handling of properties that include a reserved prefix

Reserved prefixes are:
```
/       - JSON Pointer
ext     - Common Schema extension
custom_ - Protocol -specific prefix for user-defined custom properties
```
Prefix allows to distinguish a common standard schema property from user-defined (custom) property.

## Handling of properties that do not include a reserved prefix

Properties that do not include a prefix, such as '/', 'ext.', 'custom_' - get appended to user data (custom) property bag.

Note that from SDK API layer perspective, top-level elements can be expressed as:
```
/ver
/time
/name 
```

SDK will subsequently transform those top-level properties into corresponding notation at exporter layer.

If needed, conversion from underscores '_' notation to JSON Pointer notation - forward slashes '/' - is also trivial.

## Implementation of routine that converts any property list key to JSON Pointer path

Function below accepts a property list key and transforms it into JSON Pointer format:

```cpp
std::string to_json_patch(const std::string& key, ListFormat fmt = ListFormat::JSONPath)
{
    switch (fmt)
    {
    case ListFormat::JSONPath: {
        std::string result("/");
        result += key;
        std::replace(result.begin(), result.end(), '.', '/');
        return result;
    }

    case ListFormat::UTC: {
        size_t ofs = (key.find("PartA_") == 0) ? 5 : 0;
        std::string result(key.c_str() + ofs);
        std::replace(result.begin(), result.end(), '_', '/');
        return result;
    }

    case ListFormat::Geneva: {
        size_t ofs = (key.find("env_") == 0) ? 4 : 0;
        std::string result("_ext_");
        result += std::string(key.c_str() + ofs);
        std::replace(result.begin(), result.end(), '_', '/');
        return result;
    }

    default:
        break;
    }
    return key;
}
```

# ETW layer Event property names

This example is applicable to ETW (Event Tracing for Windows exporter) flow.

## Input event map

```cpp
// Convert key-value property list to JSON
map<string, string> m =
{
    { "__ext_device_id", "c:123456" },
    { "__ext_metadata_fooBar", "one" },
    { "__ext_metadata_fooBaz", "two" }
};
auto j = to_json(m, ListFormat::UTC);
cout << j.dump(2) << endl;
```

## C++ code for each key flattening

```cpp
size_t ofs = (key.find("__") == 0) ? 2 : 0;
std::string result(key.c_str() + ofs);
std::replace(result.begin(), result.end(), '_', '/');
return result;
```

## Output after applying keys as JSON Patch

```json
{
  "ext": {
    "device": {
      "id": "c:123456"
    },
    "metadata": {
      "fooBar": "one",
      "fooBaz": "two"
    }
  }
}
```

# JSONPath Event property names

This example shows how JSONPath 'flat' property names can be converted to JSON object.

## Input event map

```cpp
// Convert from JSONPath key-value property list to JSON
cout << "**** JSONPath test:" << endl;
map<string, string> m =
{
    { "ext.device.id", "c:123456" },
    { "ext.metadata.fooBar", "one" },
    { "ext.metadata.fooBaz", "two" }
};
auto j = to_json(m, ListFormat::JSONPath);
cout << j.dump(2) << endl;
```

## C++ code for each key flattening

```cpp
std::string result("/");
result += key;
std::replace(result.begin(), result.end(), '.', '/');
return result;
```

## Output

```json
{
  "ext": {
    "device": {
      "id": "c:123456"
    },
    "metadata": {
      "fooBar": "one",
      "fooBaz": "two"
    }
  }
}
```

# Complete code example

```cpp
#include <iostream>
#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <algorithm>

using json = nlohmann::json;
using namespace std;

enum class ListFormat
{
    JSONPath = 0,
    XPath    = 1,
    ETW      = 2
};

std::string to_json_patch(const std::string& key, ListFormat fmt = ListFormat::JSONPath)
{
    switch (fmt)
    {
        case ListFormat::JSONPath: {
            std::string result("/");
            result += key;
            std::replace(result.begin(), result.end(), '.', '/');
            return result;
        }
        case ListFormat::ETW: {
            size_t ofs = (key.find("__") == 0) ? 5 : 0;
            std::string result(key.c_str() + ofs);
            std::replace(result.begin(), result.end(), '_', '/');
            return result;
        }
        case ListFormat::XPath:
            // TODO: XPath is slightly different than JSON pointer
            // with respect to array handling
        default:
            break;
    }
    return key;
}

map<string, string> to_flat_map(json & j)
{
    map<string, string> m;
    auto fj = j.flatten(); // https://tools.ietf.org/html/rfc6901
    for (json::iterator it = fj.begin(); it != fj.end(); ++it)
    {
        auto v = it.value();
        if (!v.empty())
        {
            m[it.key()] = v.dump();
        }
    }
    return m;
}

json to_json(map<string, string> m, ListFormat fmt = ListFormat::JSONPath)
{
    json result;
    for (auto &kv : m)
    {
        result[to_json_patch(kv.first, fmt)]=kv.second;
    }
    result = result.unflatten();
    return result;
}

void print_map(const map<string, string>& m)
{
    for(const auto &kv : m)
    {
        cout << kv.first << "=" << kv.second << endl;
    }
}

int main(int argc, char **argv) {

    {
        // Convert from JSON text to flat list
        cout << "**** JSON text to flat map test:" << endl;
        const char *test_json = R"JSON(
{
    "device": {
      "id": "c:123456"
    },
    "metadata": {
      "fooBar": "one",
      "fooBaz": "two"
    },
    "arrayOfStructs": [
      { "key1": "value1" },
      { "key2": "value2" },
      { "key3": "value3" }
    ],
    "foo": ["bar", "baz"],
    "": 0,
    "a/b": 1,
    "c%d": 2,
    "e^f": 3,
    "g|h": 4,
    "i\\j": 5,
    "k\"l": 6,
    " ": 7,
    "m~n": 8
}
)JSON";

        json j2 = json::parse(test_json);
        auto m = to_flat_map(j2);
        print_map(m);
    }

    {
        // Convert from JSON object to flat list
        cout << "**** JSON object to flat map test:" << endl;
        json j = {
            { "hello", "world" },
            {
                "ext",
                {
                        { "device", {} },
                        { "os", {} },
                        { "app", {} },
                }
            }
        };
        auto m = to_flat_map(j);
        print_map(m);
    }

    {
        // Convert from ETW key-value property list to JSON
        cout << "**** ETW test:" << endl;
        map<string, string> m =
        {
            { "__ext_device_id", "c:123456" },
            { "__ext_metadata_fooBar", "one" },
            { "__ext_metadata_fooBaz", "two" }
        };
        auto j = to_json(m, ListFormat::UTC);
        cout << j.dump(2) << endl;
    };

    {
        // Convert from JSONPath key-value property list to JSON
        cout << "**** JSONPath test:" << endl;
        map<string, string> m =
        {
            { "ext.device.id", "c:123456" },
            { "ext.metadata.fooBar", "one" },
            { "ext.metadata.fooBaz", "two" }
        };
        auto j = to_json(m, ListFormat::JSONPath);
        cout << j.dump(2) << endl;
    };

    return 0;
}

```
