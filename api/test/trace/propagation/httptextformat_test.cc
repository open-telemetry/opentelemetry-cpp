#include "opentelemetry/trace/propagation/httptextformat.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::trace::propagation::HTTPTextFormat;

// I am not very sure how to make tests for httpformat_test as some functions output complex objects which cannot be
// compared with some hard coded answers.
TEST(HTTPTextFormatTest, DefaultConstructionNoCrush)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
}

TEST(HTTPTextFormatTest, Extract)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();

}

TEST(HTTPTextFormatTest, Inject)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();

}

TEST(HTTPTextFormatTest, CompositeOperations)
{
    HTTPTextFormat httptextformat = HTTPTextFormat();
    Context ctx = Context();

}