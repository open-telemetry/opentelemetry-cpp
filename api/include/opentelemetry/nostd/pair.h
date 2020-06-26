#pragma once

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{

// The nostd::pair class provides a data structure capable of hold
// two values of different or the same types, and provides access to each.

template <class T1, class T2>
class pair
{
public:
  pair()
  {
    first_  = T1();
    second_ = T2();
  }

  // Constructs a pair object with the passed in first and second values.
  pair(T1 first, T2 second)
  {
    first_  = first;
    second_ = second;
  }

  void operator=(const pair &other_pair)
  {
    first_  = other_pair.first_;
    second_ = other_pair.second_;
  }

  bool operator==(const pair &other_pair)
  {
    return (first_ == other_pair.first_ && second_ == other_pair.second_);
  }

  // Returns the first element.
  T1 &first() { return first_; }

  // Returns the second element.
  T2 &second() { return second_; }

private:
  T1 first_;
  T2 second_;
};
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
