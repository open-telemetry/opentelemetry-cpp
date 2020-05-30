#pragma once

#include "opentelemetry/version.h"

#include <cstdint>
#include <cstdlib>
#include <string>

#ifdef HAVE_COLLECTIONS
#  include <bitset>
#  include <map>
#  include <vector>
#endif

#include "opentelemetry/event/Attributes.hpp"
#include "opentelemetry/event/UUID.hpp"
#include "opentelemetry/event/time_ticks.hpp"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace event
{

/// <summary>
/// The Property structure represents a C++11 variant object that holds an event property type
/// and an event property value.
/// </summary>
struct Property
{

  /// <summary>
  /// This anonymous enumeration contains a set of values that specify the types
  /// that are supported by events collector.
  /// </summary>
  enum
  {
    /// <summary>
    /// Nothing.
    /// </summary>
    TYPE_NONE,

    /// <summary>
    /// A string.
    /// </summary>
    TYPE_STRING,

    /// <summary>
    /// A 64-bit signed integer.
    /// </summary>
    TYPE_INT64,

    /// <summary>
    /// A 64-bit unsigned integer.
    /// </summary>
    TYPE_UINT64,

    /// <summary>
    /// A double.
    /// </summary>
    TYPE_DOUBLE,

    /// <summary>
    /// A date/time object.
    /// </summary>
    TYPE_TIME,

    /// <summary>
    /// A boolean.
    /// </summary>
    TYPE_BOOLEAN,

    /// <summary>
    /// UUID.
    /// </summary>
    TYPE_UUID,

#ifdef HAVE_COLLECTIONS
    /// <summary>
    /// Reserved for complex types.
    /// </summary>
    TYPE_OBJECT,

    /// <summary>String</summary>
    TYPE_STRING_ARRAY,

    /// <summary>64-bit signed integer array</summary>
    TYPE_INT64_ARRAY,

    /// <summary>64-bit unsigned integer array</summary>
    TYPE_UINT64_ARRAY,

    /// <summary>double</summary>
    TYPE_DOUBLE_ARRAY,

    /// <summary>GUID</summary>
    TYPE_UUID_ARRAY,

#endif

  } type;

  /// <summary>
  /// Event property attributes (metadata).
  /// </summary>
  Attributes attribs;

  /// <summary>
  /// Variant object value - 128-bit wide
  /// </summary>
  // clang-format off
  union
  {
    char*                       as_string;
    int64_t                     as_int64;
    uint64_t                    as_uint64;
    double                      as_double;
    time_ticks                  as_time;
    bool                        as_bool;
    UUID                        as_uuid;
    void*                       as_object;
#ifdef HAVE_COLLECTIONS
    std::bitset<128>*           as_bitset128;
    std::vector<int64_t>*       as_longArray;
    std::vector<double>*        as_doubleArray;
    std::vector<UUID>*          as_uuidArray;
    std::vector<std::string>*   as_stringArray;
#endif
  };
  // clang-format on

#ifndef NDEBUG
  /// <summary>
  /// Debug routine that returns string representation of type name
  /// </summary>
  static const char *type_name(unsigned typeId)
  {
    // clang-format off
      static const char* type_names[] =
      {
          "string",
          "int64",
          "uint64",
          "double",
          "time",
          "boolean",
          "uuid",
#ifdef HAVE_COLLECTIONS
          "object",
          "bitset128",
          "array_long",
          "array_double",
          "array_uuid",
          "array_string"
#endif
      };
    // clang-format on

    // TODO: add safety check with ARRAY_SIZEOF
    return type_names[typeId];
  }
#endif

  void copyfrom(Property const *source)
  {
    switch (type)
    {
      case TYPE_STRING: {
        if (source->as_string != nullptr)
        {
          // Assignment from static initializer Property creates a buffer copy
          size_t len = strlen(source->as_string);
          as_string  = new char[len + 1];
          memcpy((void *)as_string, (void *)source->as_string, len);
          as_string[len] = 0;
        }
        else
        {
          as_string = nullptr;
        }
        break;
      }

      case TYPE_INT64: {
        as_int64 = source->as_int64;
        break;
      }

      case TYPE_UINT64: {
        as_uint64 = source->as_uint64;
        break;
      }

      case TYPE_DOUBLE: {
        as_double = source->as_double;
        break;
      }

      case TYPE_TIME: {
        as_time = source->as_time;
        break;
      }

      case TYPE_BOOLEAN: {
        as_bool = source->as_bool;
        break;
      }

      case TYPE_UUID: {
        as_uuid = source->as_uuid;
        break;
      }

#ifdef HAVE_COLLECTIONS
      /**
       * TODO:
       * - add Object "T"
       * - add bitset
       */
      case TYPE_INT64_ARRAY: {
        as_longArray = new std::vector<int64_t>(*source->as_longArray);
        break;
      }

      case TYPE_DOUBLE_ARRAY: {
        as_doubleArray = new std::vector<double>(*source->as_doubleArray);
        break;
      }

      case TYPE_GUID_ARRAY: {
        as_uuidArray = new std::vector<GUID_t>(*source->as_uuidArray);
        break;
      }

      case TYPE_STRING_ARRAY: {
        as_stringArray = new std::vector<std::string>(*source->as_stringArray);
        break;
      }
#endif

      default:
        /* TODO: assert with unsupported type */
        break;
    }
  }

  /// <summary>
  /// Property copy constructor
  /// </summary>
  /// <param name="source">Right-hand side value of object</param>
  Property(const Property &source)
  {
    // TODO: avoid memcpy here, seems unnecessary
    memcpy((void *)this, (void *)&source, sizeof(Property));
    copyfrom(&source);
  }

  /// <summary>
  /// The Property move constructor.
  /// </summary>
  /// <param name="source">The Property object to move.</param>
  Property(Property &&source)
  {
    // TODO: avoid memcpy here, seems unnecessary
    memcpy((void *)this, (void *)&source, sizeof(Property));
    copyfrom(&source);
  }

  /// <summary>
  /// The Property equalto operator.
  /// </summary>
  bool operator==(const Property &source) const
  {
    if (attribs != source.attribs)
    {
      return false;
    }

    if (type == source.type)
    {
      switch (type)
      {

        case TYPE_STRING: {
          // TODO: avoid buffer copy here
          std::string temp1 = as_string;
          std::string temp2 = source.as_string;
          if (temp1.compare(temp2) == 0)
          {
            return true;
          }
          break;
        }

        case TYPE_INT64:
          if (as_int64 == source.as_int64)
          {
            return true;
          }
          break;

        case TYPE_UINT64:
          if (as_uint64 == source.as_uint64)
          {
            return true;
          }
          break;

        case TYPE_DOUBLE:
          if (as_double == source.as_double)
          {
            return true;
          }
          break;

        case TYPE_TIME:
          if (as_time.ticks == source.as_time.ticks)
          {
            return true;
          }
          break;

        case TYPE_BOOLEAN:
          if (as_bool == source.as_bool)
          {
            return true;
          }
          break;

        case TYPE_UUID: {
          // TODO: avoid string conversion here
          std::string temp1 = as_uuid.to_string();
          std::string temp2 = source.as_uuid.to_string();
          if (temp1.compare(temp2) == 0)
          {
            return true;
          }
          break;
        }

#ifdef HAVE_COLLECTIONS
        case TYPE_INT64_ARRAY: {
          if (*as_longArray == *source.as_longArray)
          {
            return true;
          }
          break;
        }

        case TYPE_DOUBLE_ARRAY: {
          if (*as_doubleArray == *source.as_doubleArray)
          {
            return true;
          }
          break;
        }

        case TYPE_GUID_ARRAY: {
          if (*as_guidArray == *source.as_guidArray)
          {
            return true;
          }
          break;
        }

        case TYPE_STRING_ARRAY: {
          if (*as_stringArray == *source.as_stringArray)
          {
            return true;
          }
          break;
        }
#endif
        default:
          break;
      }
    }
    return false;
  }

  /// <summary>
  /// An Property assignment operator that takes an Property object.
  /// </summary>
  Property &operator=(const Property &source)
  {
    clear();
    memcpy((void *)this, (void *)&source, sizeof(Property));
    copyfrom(&source);
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes a string value.
  /// </summary>
  Property &operator=(const std::string &value)
  {
    clear();
    size_t len = strlen(value.c_str());
    as_string  = new char[len + 1];
    memcpy((void *)as_string, (void *)value.c_str(), len);
    as_string[len] = 0;
    type           = TYPE_STRING;
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes a character pointer to a string.
  /// </summary>
  Property &operator=(const char *value)
  {
    clear();
    size_t len = strlen(value);
    as_string  = new char[len + 1];
    memcpy((void *)as_string, (void *)value, len);
    as_string[len] = 0;
    type           = TYPE_STRING;
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes an int64_t value.
  /// </summary>
  Property &operator=(int64_t value)
  {
    clear();
    this->type     = TYPE_INT64;
    this->as_int64 = value;
    return (*this);
  }

#ifndef LONG_IS_INT64_T
  Property &operator=(long value) { return ((*this) = static_cast<int64_t>(value)); }
#endif

  /// <summary>
  /// An Property assignment operator that takes an int8_t value.
  /// </summary>
  Property &operator=(int8_t value) { return ((*this) = static_cast<int64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes an int16_t value.
  /// </summary>
  Property &operator=(int16_t value) { return ((*this) = static_cast<int64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes an int32_t value.
  /// </summary>
  Property &operator=(int32_t value) { return ((*this) = static_cast<int64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes a uint8_t value.
  /// </summary>
  Property &operator=(uint8_t value) { return ((*this) = static_cast<uint64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes a uint16_t value.
  /// </summary>
  Property &operator=(uint16_t value) { return ((*this) = static_cast<uint64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes a uint32_t value.
  /// </summary>
  Property &operator=(uint32_t value) { return ((*this) = static_cast<uint64_t>(value)); }

  /// <summary>
  /// An Property assignment operator that takes a uint64_t value.
  /// </summary>
  Property &operator=(uint64_t value) { return ((*this) = static_cast<uint64_t>(value)); }

#ifdef HAVE_COLLECTIONS
  // TODO:
  // - add collections assignment operators
  // - add support for 128-bit bitset
  Property &operator=(const std::vector<int64_t> &value)
  {
    clear();
    type         = TYPE_INT64_ARRAY;
    as_longArray = new std::vector<int64_t>(value);
    return (*this);
  }

  Property &operator=(const std::vector<double> &value)
  {
    clear();
    type           = TYPE_DOUBLE_ARRAY;
    as_doubleArray = new std::vector<double>(value);
    return (*this);
  }

  Property &operator=(const std::vector<UUID> &value)
  {
    clear();
    type         = TYPE_UUID_ARRAY;
    as_guidArray = new std::vector<UUID>(value);
    return (*this);
  }

  Property &operator=(const std::vector<std::string> &value)
  {
    clear();
    type           = TYPE_STRING_ARRAY;
    as_stringArray = new std::vector<std::string>(value);
    return (*this);
  }

#endif

  /// <summary>
  /// An Property assignment operator that takes a double.
  /// </summary>
  Property &operator=(double value)
  {
    clear();
    this->type      = TYPE_DOUBLE;
    this->as_double = value;
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes a boolean value.
  /// </summary>
  Property &operator=(bool value)
  {
    clear();
    this->type    = TYPE_BOOLEAN;
    this->as_bool = value;
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes a time_ticks_t value.
  /// </summary>
  Property &operator=(time_ticks value)
  {
    clear();
    this->type    = TYPE_TIME;
    this->as_time = value;
    return (*this);
  }

  /// <summary>
  /// An Property assignment operator that takes a UUID value.
  /// </summary>
  Property &operator=(UUID value)
  {
    clear();
    this->type    = TYPE_UUID;
    this->as_uuid = value;
    return (*this);
  }

  /// <summary>
  /// Clears the object values, deallocating memory when needed.
  /// </summary>
  void clear()
  {
    switch (type)
    {
      case TYPE_STRING: {
        if (as_string != nullptr)
        {
          delete[] as_string;
          as_string = nullptr;
        }
        break;
      }
#ifdef HAVE_COLLECTIONS
      case TYPE_INT64_ARRAY: {
        if (as_longArray != nullptr)
        {
          delete as_longArray;
          as_longArray = nullptr;
        }
        break;
      }

      case TYPE_DOUBLE_ARRAY: {
        if (as_doubleArray != nullptr)
        {
          delete as_doubleArray;
          as_doubleArray = nullptr;
        }
        break;
      }
      case TYPE_GUID_ARRAY: {
        if (as_uuidArray != nullptr)
        {
          delete as_uuidArray;
          as_uuidArray = nullptr;
        }
        break;
      }
      case TYPE_STRING_ARRAY: {
        if (as_stringArray != nullptr)
        {
          delete as_stringArray;
          as_stringArray = nullptr;
        }
        break;
      }
#endif
      default:
        break;  // nothing to delete
    }
    attribs = Attribute::ATTRIB_NONE;
  }

  /// <summary>
  /// The Property destructor.
  /// </summary>
  virtual ~Property() { clear(); }

  /// <summary>
  /// Empty Property default constructor.
  /// </summary>
  Property() : type(TYPE_NONE), attribs(Attribute::ATTRIB_NONE), as_object(nullptr){};

  /// <summary>
  /// The Property constructor, taking a character pointer to a string, and the kind of
  /// personal identifiable information.
  /// </summary>
  /// <param name="value">A constant character pointer to a string.</param>
  /// <param name="attribs">The kind of personal identifiable information.</param>
  /// Property constructor for string value
  /// </summary>
  /// <param name="value">string value</param>
  /// <param name="attribs">Pii kind</param>
  Property(const char *value, Attributes attribs = Attribute::Attribute::ATTRIB_NONE)
      : type(TYPE_STRING), attribs(attribs)
  {
    if (nullptr == value)
    {
      as_string    = new char[1];
      as_string[0] = 0;
    }
    else
    {
      size_t len = strlen(value);
      as_string  = new char[len + 1];
      // TODO: throws if OOM?
      memcpy((void *)as_string, (void *)value, len);
      as_string[len] = 0;
    }
  }

  /// <summary>
  /// Property constructor for string value
  /// </summary>
  /// <param name="value">string value</param>
  /// <param name="attribs">Pii kind</param>
  Property(const std::string &value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_STRING), attribs(attribs)
  {
    size_t len = strlen(value.c_str());
    as_string  = new char[len + 1];
    // TODO: throws if OOM?
    memcpy((void *)as_string, (void *)value.c_str(), len);
    as_string[len] = 0;
  }

  /// <summary>
  /// Property constructor for double value
  /// </summary>
  /// <param name="value">double value</param>
  /// <param name="attribs">Pii kind</param>
  Property(double value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_DOUBLE), attribs(attribs), as_double(value){};

  /// <summary>
  /// Property constructor for time in .NET ticks
  /// </summary>
  /// <param name="value">time_ticks_t value - time in .NET ticks</param>
  /// <param name="attribs">Pii kind</param>
  Property(time_ticks value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_TIME), attribs(attribs), as_time(value){};

  /// <summary>
  /// Property constructor for boolean value
  /// </summary>
  /// <param name="value">boolean value</param>
  /// <param name="attribs">Pii kind</param>
  Property(bool value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_BOOLEAN), attribs(attribs), as_bool(value){};

  /// <summary>
  /// Property constructor for GUID
  /// </summary>
  /// <param name="value">UUID value</param>
  /// <param name="attribs">Pii kind</param>
  Property(UUID value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_UUID), attribs(attribs), as_uuid(value){};

  /// <summary>
  /// Property constructor that takes an int8_t value and Attributes.
  /// </summary>
  Property(int8_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_int64(value){};

  /// <summary>
  /// Property constructor that takes an int16_t value and Attributes.
  /// </summary>
  Property(int16_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_int64(value){};

  /// <summary>
  /// Property constructor that takes an int32_t value and Attributes.
  /// </summary>
  Property(int32_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_int64(value){};

  /// <summary>
  /// Property constructor that takes an int64_t value and Attributes.
  /// </summary>
  Property(int64_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_int64(value){};

#ifndef LONG_IS_INT64_T
  /// <summary>
  /// Property constructor that takes a long value and Attributes.
  /// </summary>
  Property(long value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_int64(value){};
#endif

  /// <summary>
  /// Property constructor that takes an uint8_t value and Attributes.
  /// </summary>
  Property(uint8_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_uint64(value){};

  /// <summary>
  /// Property constructor that takes an uint16_t value and Attributes.
  /// </summary>
  Property(uint16_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_uint64(value){};

  /// <summary>
  /// Property constructor that takes an uint32_t value and Attributes.
  /// </summary>
  Property(uint32_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_uint64(value){};

  /// <summary>
  /// Property constructor that takes an uint64_t value and Attributes.
  /// </summary>
  Property(uint64_t value, Attributes attribs = Attribute::ATTRIB_NONE)
      : type(TYPE_INT64), attribs(attribs), as_uint64(value){};

#ifdef HAVE_COLLECTIONS
  Property(std::vector<int64_t> &value, Attributes attribs = Attribute::ATTRIB_NONE)
  {
    // TODO
  }

  Property(std::vector<double> &value, Attributes attribs = Attribute::ATTRIB_NONE)
  {
    // TODO
  }

  Property(std::vector<UUID> &value, Attributes attribs = Attribute::ATTRIB_NONE)
  {
    // TODO
  }

  Property(std::vector<std::string> &value, Attributes attribs = Attribute::ATTRIB_NONE)
  {
    // TODO
  }
#endif

  /// <summary>
  /// Returns <i>true</i> when the property is empty.
  /// </summary>
  bool empty()
  {
    return ((type == TYPE_STRING) && (as_string != nullptr) && (as_string[0] == 0)) ||
           (type == TYPE_NONE);
  }

  /// <summary>Return a string representation of this value object</summary>
  std::string to_string() const
  {
    std::string result;
    switch (type)
    {
      case TYPE_STRING:
        result = as_string;
        break;
      case TYPE_INT64:
        result = std::to_string(as_int64);
        break;
      case TYPE_DOUBLE:
        result = std::to_string(as_double);
        break;
      case TYPE_TIME:
        // Note that we do not format time as time, we return it as raw number of .NET ticks
        result = std::to_string(as_time.ticks);
        break;
      case TYPE_BOOLEAN:
        result = ((as_bool) ? "true" : "false");
        break;
      case TYPE_UUID:
        result = as_uuid.to_string();
        break;

#ifdef HAVE_COLLECTIONS
      case TYPE_INT64_ARRAY: {
        if (as_longArray != NULL)
        {
          stringstream ss;
          for (int64_t element : *as_longArray)
          {
            ss << element;
            ss << ",";
          }
          string s = ss.str();
          result   = s.substr(0, s.length() - 1);  // get rid of the trailing space
        }
        break;
      }
      case TYPE_DOUBLE_ARRAY: {
        if (as_doubleArray != NULL)
        {
          stringstream ss;
          for (double element : *as_doubleArray)
          {
            ss << element;
            ss << ",";
          }
          string s = ss.str();
          result   = s.substr(0, s.length() - 1);  // get rid of the trailing space
        }
        break;
      }
      case TYPE_GUID_ARRAY: {
        if (as_guidArray != NULL)
        {
          stringstream ss;
          for (const auto &element : *as_guidArray)
          {
            ss << element.to_string();
            ss << ",";
          }
          string s = ss.str();
          result   = s.substr(0, s.length() - 1);  // get rid of the trailing space
        }
        break;
      }
      case TYPE_STRING_ARRAY: {
        if (as_stringArray != NULL)
        {
          stringstream ss;
          for (const auto &element : *as_stringArray)
          {
            ss << element;
            ss << ",";
          }
          string s = ss.str();
          result   = s.substr(0, s.length() - 1);  // get rid of the trailing space
        }
        break;
      }
#endif
      default:
        result = "";
        break;
    }
    return result;
  }

  /// <summary>
  /// Returns a string representation of this object.
  /// </summary>
  // virtual std::string to_string() const;
};

}  // namespace event

OPENTELEMETRY_END_NAMESPACE
