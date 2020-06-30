#include "opentelemetry/trace/propagation/httptextformat.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::trace::propagation::HTTPTextFormat;

nostd::string_view getter(std::map<std::string,std::string> &carrier, nostd::string_view trace_type = "traceparent") {
    nostd::string_view res = carrier["version"]+"-"+carrier["trace-id"]+"-"+carrier["parent-id"]+"-"+carrier["trace-flags"];
    return res;
}

void setter(std::map<char,int> &carrier, nostd::string_view trace_type = "traceparent", nostd::string trace_description = "") {
    nostd::string_view str = "";
    int s = 0;
    for (int i = 0; i < trace_description.length(); i++) {
        if (trace_description[i]=='-') {
            if (s == 0) carrier["version"] = str;
            if (s == 1) carrier["trace-id"] = str;
            if (s == 2) carrier["parent-id"] = str;
            str.clear();
        } else {
            str.push_back(trace_description[i]);
        }
    }
    carrier["trace-flags"] = str;
}

// I am not very sure how to make tests for httpformat_test as some functions output complex objects which cannot be
// compared with some hard coded answers.
TEST(HTTPTextFormatTest, DefaultConstructionNoCrush)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
}

TEST(HTTPTextFormatTest, ExtractNoCrush)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();
    std::map<char,int> carrier;
    extract(getter, carrier, ctx);
}

TEST(HTTPTextFormatTest, InjectNoCrush)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();
    std::map<char,int> carrier = {{'1',1},{'2',2},{'3',3}};
    inject(setter, carrier, ctx);
}

// In this test we want to see the after injecting a header's information into a context,
// when we extract it it will give use back the same information for the header info.
TEST(HTTPTextFormatTest, CompositeOperations)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();
    std::map<char,int> carrier = {{'1',1},{'2',2},{'3',3}};
    inject(setter, carrier, ctx);
    std::map<char,int> res;
    extract(getter, res, ctx);
    EXPECT_EQ(res, carrier);
}