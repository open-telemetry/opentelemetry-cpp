// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#include <vector>
#if (__GNUC__ == 4 && (__GNUC_MINOR__ == 8 || __GNUC_MINOR__ == 9))
#  define HAVE_WORKING_REGEX 0
#else
#include <regex>
#  define HAVE_WORKING_REGEX 1
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class Predicate {
    public:
    virtual ~Predicate() = default;
    virtual bool Match(nostd::string_view string) = 0;
}

class PatternPredicate : public Predicate{
public:
    PatternPredicate(nostd::string_view pattern): reg_key{pattern}{
        if (pattern == "*"){
            matchAll_ = true;
        }
        else {
            matchAll_ = false;
        }
    }
    virtual bool Match(std::string str){
        if (matchAll_){
            return true;
        } else {
#if HAVE_WORKING_REGEX
            return std::regex_match(str, reg_key_);
#else
            return false; // not supported
        }
    }
private:
    bool matchAll_;
#ifdef HAVE_WORKING_REGEX
    std::regex reg_key_;
#else
    std::string reg_key_;
#endif
}

class ExactPredicate : public Predicate {
public:
    ExactPredicate(nostd::string_view pattern):pattern{pattern_}
    {
        matchAll_ = pattern_.size() ? true: false;

    }
    virtual bool Match(nostd::string_view str){
        if (matchAll_){
            return true;
        }
        if (pattern_ == str){
            return true;
        } else {
            return false;
        }
    }

private:
    bool matchAll_;
    std::string pattern_;
};


}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
