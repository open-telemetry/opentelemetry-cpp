// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <iostream>
#include <sstream>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace ostream
{
namespace test
{
/**
 * The OStreamCapture captures from the specified stream for its lifetime
 */
class OStreamCapture
{
public:
  /**
   * Create a OStreamCapture which will capture the output of the ostream that it was constructed
   * with for the lifetime of the instance.
   */
  OStreamCapture(std::ostream &ostream) : stream_(ostream), buf_(ostream.rdbuf())
  {
    stream_.rdbuf(captured_.rdbuf());
  }

  ~OStreamCapture() { stream_.rdbuf(buf_); }

  /**
   * Returns the captured data from the stream.
   */
  std::string GetCaptured() const { return captured_.str(); }

private:
  std::ostream &stream_;
  std::streambuf *buf_;
  std::stringstream captured_;
};

/**
 * Helper method to invoke the passed func while recording the output of the specified stream and
 * return the output afterwards.
 */
template <typename Func>
std::string WithOStreamCapture(std::ostream &stream, Func func)
{
  OStreamCapture capture(stream);
  func();
  return capture.GetCaptured();
}

}  // namespace test
}  // namespace ostream
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
