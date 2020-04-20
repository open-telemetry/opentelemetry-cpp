#pragma once

#include "opentelemetry/version.h"

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <cstdint>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace event {

    /// <summary>
    /// The number of ticks per second.
    /// </summary>
    const uint64_t ticksPerSecond = 10000000UL;

    /// <summary>
    /// The UNIX epoch: Thursday, January, 01, 1970, 12:00:00 AM.
    /// </summary>
    const uint64_t ticksUnixEpoch = 0x089f7ff5f7b58000;

    /// <summary>
    /// The time_ticks_t structure encapsulates time in .NET ticks.
    /// </summary>
    /// <remarks>
    /// A single tick represents one hundred nanoseconds, or one ten-millionth of a second.
    /// There are 10,000 ticks in a millisecond, or 10 million ticks in a second.
    /// The value of this property represents the number of 100 nanosecond intervals that have
    /// elapsed since 12:00 AM, January, 1, 0001 (0:00 : 00 UTC on January 1, 0001, in
    /// the Gregorian calendar), which represents DateTime.MinValue.
    /// <b>Note:</b> This does not include the number  of ticks that are attributable to leap seconds.
    /// </remarks>
    struct MATSDK_LIBABI time_ticks_t {
        /// <summary>
        /// A raw 64-bit unsigned integer that represents the number of .NET ticks.
        /// </summary>
        uint64_t ticks;

        /// <summary>
        /// The default constructor for instantiating an empty time_ticks_t object.
        /// </summary>
        time_ticks_t();

        /// <summary>
        /// Converts the number of .NET ticks into an instance of the time_ticks_t structure.
        /// </summary>
        time_ticks_t(uint64_t raw);

        /// <summary>
        /// Constructs a time_ticks_t object from a pointer to a time_t object from the standard library.
        /// <b>Note:</b> time_t time must contain a timestamp in UTC time.
        /// </summary>
        time_ticks_t(const std::time_t* time);

        /// <summary>
        /// The time_ticks_t copy constructor.
        /// </summary>
        time_ticks_t(const time_ticks_t& t);
    };

    /// <summary>
    /// The GUID_t structure represents the portable cross-platform implementation of a GUID (Globally Unique ID).
    /// </summary>
    /// <remarks>
    /// GUIDs identify objects such as interfaces, manager entry-point vectors (EPVs), and class objects.
    /// A GUID is a 128-bit value consisting of one group of eight hexadecimal digits, followed
    /// by three groups of four hexadecimal digits, each followed by one group of 12 hexadecimal digits.
    /// 
    /// The definition of this structure is the cross-platform equivalent to the 
    /// [Windows RPC GUID definition](https://msdn.microsoft.com/en-us/library/windows/desktop/aa373931%28v=vs.85%29.aspx).
    /// 
    /// <b>Note:</b> You must provide your own converter to convert from a <b>Windows RPC GUID</b> to a GUID_t.
    /// </remarks>
    struct MATSDK_LIBABI GUID_t {
        /// <summary>
        /// Specifies the first eight hexadecimal digits of the GUID.
        /// </summary>
        uint32_t Data1;

        /// <summary>
        /// Specifies the first group of four hexadecimal digits.
        ///</summary>
        uint16_t Data2;

        /// <summary>
        /// Specifies the second group of four hexadecimal digits.
        /// </summary>
        uint16_t Data3;

        /// <summary>
        /// An array of eight bytes.
        /// The first two bytes contain the third group of four hexadecimal digits.
        /// The remaining six bytes contain the final 12 hexadecimal digits.
        /// </summary>
        uint8_t  Data4[8];

        /// <summary>
        /// The default GUID_t constructor.
        /// Creates a null instance of the GUID_t object (initialized to all zeros).
        /// {00000000-0000-0000-0000-000000000000}.
        /// </summary>
        GUID_t();

        /// <summary>
        /// A constructor that creates a GUID_t object from a hyphenated string.
        /// </summary>
        /// <param name="guid_string">A hyphenated string that contains the GUID (curly braces optional).</param>
        GUID_t(const char* guid_string);

        /// <summary>
        /// A constructor that creates a GUID_t object from a byte array.
        /// </summary>
        /// <param name="guid_bytes">A byte array.</param>
        /// <param name="bigEndian">
        /// A boolean value that specifies the byte order.<br>
        /// A value of <i>true</i> specifies the more natural human-readable order.<br>
        /// A value of <i>false</i> (the default) specifies the same order as the .NET GUID constructor.
        /// </param>
        GUID_t(const uint8_t guid_bytes[16], bool bigEndian = false);

        /// <summary>
        /// A constructor that creates a GUID_t object from three integers and a byte array.
        /// </summary>
        /// <param name="d1">An integer that specifies the first eight hexadecimal digits of the GUID.</param>
        /// <param name="d2">An integer that specifies the first group of four hexadecimal digits.</param>
        /// <param name="d3">An integer that specifies the second group of four hexadecimal digits.</param>
        /// <param name="v">A reference to an array of eight bytes.
        /// The first two bytes contain the third group of four hexadecimal digits.
        /// The remaining six bytes contain the final 12 hexadecimal digits.
        /// </param>
        GUID_t(int d1, int d2, int d3, const std::initializer_list<uint8_t> &v);

        /// <summary>
        /// The GUID_t copy constructor.
        /// </summary>
        /// <param name="guid">A GUID_t object.</param>
        GUID_t(const GUID_t& guid);

#ifdef _WIN32
        /// <summary>
        /// A constructor that creates a GUID_t object from a Windows GUID object.
        /// </summary>
        /// <param name="guid">A Windows GUID object.</param>
        GUID_t(GUID guid);

        /// <summary>
        /// Converts a standard vector of bytes into a Windows GUID object.
        /// </summary>
        /// <param name="bytes">A standard vector of bytes.</param>
        /// <returns>A GUID.</returns>
        static GUID convertUintVectorToGUID(std::vector<uint8_t> const& bytes);

#endif
        /// <summary>
        /// Converts this GUID_t to an array of bytes.
        /// </summary>
        /// <param name="guid_bytes">A uint8_t array of 16 bytes.</param>
        void to_bytes(uint8_t(&guid_bytes)[16]) const;

        /// <summary>
        /// Convert this GUID_t object to a string.
        /// </summary>
        /// <returns>This GUID_t object in a string.</returns>
        std::string to_string() const;

        /// <summary>
        /// Calculates the size of this GUID_t object.
        /// The output from this method is compatible with std::unordered_map.
        /// </summary>
        /// <returns>The size of the GUID_t object in bytes.</returns>
        std::size_t Hash() const;

        /// <summary>
        /// Tests to determine whether two GUID_t objects are equivalent (needed for maps).
        /// </summary>
        /// <returns>A boolean value that indicates success or failure.</returns>
        bool operator==(GUID_t const& other) const;

        /// <summary>
        /// Tests to determine how to sort 2 GUID_t objects
        /// </summary>
        /// <returns>A boolean value that indicates success or failure.</returns>
        bool operator<(GUID_t const& other) const;
    };

    /// @cond INTERNAL_DOCS
    /// Excluded from public docs
    /// <summary>
    /// Declare GuidComparer as the Comparer when using GUID_t as a key in a map or set
    /// </summary>
    struct GuidComparer : std::less<GUID_t>
    {
        inline std::size_t operator()(GUID_t const& key) const
        {
            return key.Hash();
        }

        inline bool operator()(GUID_t const& lhs, GUID_t const& rhs) const
        {
            return lhs.Hash() < rhs.Hash();
        }
    };
    /// @endcond

    /// <summary>
    /// The EventProperty structure represents a C++11 variant object that holds an event property type 
    /// and an event property value.
    /// </summary>
    struct MATSDK_LIBABI EventProperty
    {
        // <remarks>
        // With the concept of EventProperty value object we allow users implementing their
        // own type conversion system, which may subclass and provides an implementation of
        // to_string method
        // </remarks>
    public:

        /// <summary>
        /// This anonymous enumeration contains a set of values that specify the types
        /// that are supported by events collector.
        /// </summary>
        enum
        {
            /// <summary>
            /// A string.
            /// </summary>
            TYPE_STRING,
            /// <summary>
            /// A 64-bit signed integer.
            /// </summary>
            TYPE_INT64,
            /// <summary>
            /// A double.
            /// </summary>
            TYPE_DOUBLE,
            /// <summary>
            /// A date/time object represented in .NET ticks.
            /// </summary>
            TYPE_TIME,
            /// <summary>
            /// A boolean.
            /// </summary>
            TYPE_BOOLEAN,
            /// <summary>
            /// A GUID.
            /// </summary>
            TYPE_GUID,
            /// <summary>String</summary>
            TYPE_STRING_ARRAY,
            /// <summary>64-bit signed integer</summary>
            TYPE_INT64_ARRAY,
            /// <summary>double</summary>
            TYPE_DOUBLE_ARRAY,
            /// <summary>GUID</summary>
            TYPE_GUID_ARRAY,
        } type;

        /// <summary>
        /// The kind of PII (Personal Identifiable Information) for an event.
        /// </summary>
        PiiKind piiKind;

        /// <summary>
        DataCategory dataCategory = DataCategory_PartC;

        /// <summary>
        /// Variant object value
        /// </summary>
        union
        {
            char*        as_string;
            int64_t      as_int64;
            double       as_double;
            bool         as_bool;
            GUID_t       as_guid;
            time_ticks_t as_time_ticks;
            std::vector<int64_t>*     as_longArray;
            std::vector<double>*      as_doubleArray;
            std::vector<GUID_t>*      as_guidArray;
            std::vector<std::string>* as_stringArray;
        };

        /// <summary>Debug routine that returns string representation of type name</summary>
        static const char *type_name(unsigned typeId);

        /// <summary>
        /// EventProperty copy constructor
        /// </summary>
        /// <param name="source">Right-hand side value of object</param>
        EventProperty(const EventProperty& source);

        /// <summary>
        /// The EventProperty move constructor.
        /// </summary>
        /// <param name="source">The EventProperty object to move.</param>
        EventProperty(EventProperty&& source);

        /// <summary>
        /// The EventProperty equalto operator.
        /// </summary>
        bool operator==(const EventProperty& source) const;

        /// <summary>
        /// An EventProperty assignment operator that takes an EventProperty object.
        /// </summary>
        EventProperty& operator=(const EventProperty& source);

        /// <summary>
        /// An EventProperty assignment operator that takes a string value.
        /// </summary>
        EventProperty& operator=(const std::string& value);

        /// <summary>
        /// An EventProperty assignment operator that takes a character pointer to a string.
        /// </summary>
        EventProperty& operator=(const char *value);

        /// <summary>
        /// An EventProperty assignment operator that takes an int64_t value.
        /// </summary>
        EventProperty& operator=(int64_t value);

        // All other integer types get converted to int64_t
#ifndef LONG_IS_INT64_T
        EventProperty& operator=(long    value);
#endif
        /// <summary>
        /// An EventProperty assignment operator that takes an int8_t value.
        /// </summary>
        EventProperty& operator=(int8_t  value);

        /// <summary>
        /// An EventProperty assignment operator that takes an int16_t value.
        /// </summary>
        EventProperty& operator=(int16_t value);

        /// <summary>
        /// An EventProperty assignment operator that takes an int32_t value.
        /// </summary>
        EventProperty& operator=(int32_t value);

        /// <summary>
        /// An EventProperty assignment operator that takes a uint8_t value.
        /// </summary>
        EventProperty& operator=(uint8_t  value);

        /// <summary>
        /// An EventProperty assignment operator that takes a uint16_t value.
        /// </summary>
        EventProperty& operator=(uint16_t value);

        /// <summary>
        /// An EventProperty assignment operator that takes a uint32_t value.
        /// </summary>
        EventProperty& operator=(uint32_t value);

        /// <summary>
        /// An EventProperty assignment operator that takes a uint64_t value.
        /// </summary>
        EventProperty& operator=(uint64_t value);

        EventProperty& operator=(const std::vector<int64_t>& value);

        EventProperty& operator=(const std::vector<double>& value);

        EventProperty& operator=(const std::vector<GUID_t>& value);

        EventProperty& operator=(const std::vector<std::string>& value);

        /// <summary>
        /// An EventProperty assignment operator that takes a double.
        /// </summary>
        EventProperty& operator=(double value);

        /// <summary>
        /// An EventProperty assignment operator that takes a boolean value.
        /// </summary>
        EventProperty& operator=(bool value);

        /// <summary>
        /// An EventProperty assignment operator that takes a time_ticks_t value.
        /// </summary>
        EventProperty& operator=(time_ticks_t value);

        /// <summary>
        /// An EventProperty assignment operator that takes a GUID_t value.
        /// </summary>
        EventProperty& operator=(GUID_t value);

        /// <summary>
        /// Clears the object values, deallocating memory when needed.
        /// </summary>
        void clear();

        /// <summary>
        /// The EventProperty destructor.
        /// </summary>
        virtual ~EventProperty();

        /// <summary>
        /// The EventProperty default constructor.
        /// </summary>
        EventProperty();

        /// <summary>
        /// The EventProperty constructor, taking a character pointer to a string, and the kind of personal identifiable information.
        /// </summary>
        /// <param name="value">A constant character pointer to a string.</param>
        /// <param name="piiKind">The kind of personal identifiable information.</param>
                    /// EventProperty constructor for string value
                    /// </summary>
                    /// <param name="value">string value</param>
                    /// <param name="piiKind">Pii kind</param>
        EventProperty(const char* value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for string value
        /// </summary>
        /// <param name="value">string value</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(const std::string& value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for int64 value
        /// </summary>
        /// <param name="value">int64_t value</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(int64_t       value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for double value
        /// </summary>
        /// <param name="value">double value</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(double        value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for time in .NET ticks
        /// </summary>
        /// <param name="value">time_ticks_t value - time in .NET ticks</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(time_ticks_t  value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for boolean value
        /// </summary>
        /// <param name="value">boolean value</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(bool          value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        /// <summary>
        /// EventProperty constructor for GUID
        /// </summary>
        /// <param name="value">GUID_t value</param>
        /// <param name="piiKind">Pii kind</param>
        EventProperty(GUID_t        value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        // All other integer types get converted to int64_t
#ifndef LONG_IS_INT64_T
        EventProperty(long     value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);
#endif

        EventProperty(int8_t   value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(int16_t  value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(int32_t  value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(uint8_t  value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(uint16_t value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(uint32_t value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(uint64_t value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(std::vector<int64_t>& value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(std::vector<double>& value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(std::vector<GUID_t>& value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);

        EventProperty(std::vector<std::string>& value, PiiKind piiKind = PiiKind_None, DataCategory category = DataCategory_PartC);
        /// <summary>
        /// Returns <i>true</i> when the type is string AND the value is empty.
        /// </summary>
        bool empty();

        /// <summary>
        /// Returns a string representation of this object.
        /// </summary>
        virtual std::string to_string() const;

    private:
        void copydata(EventProperty const* source);

    };

} ARIASDK_NS_END
