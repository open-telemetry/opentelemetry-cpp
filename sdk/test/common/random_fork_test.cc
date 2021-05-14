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

#ifdef __unix__
// Verifies that IDs don't clash after forking the process.
//
// See https://github.com/opentracing-contrib/nginx-opentracing/issues/52
#  include "src/common/random.h"

#  include <sys/mman.h>
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#  include <cstdio>
#  include <cstdlib>
#  include <iostream>
using opentelemetry::sdk::common::Random;

static uint64_t *child_id;

int main()
{
  // Set up shared memory to communicate between parent and child processes.
  //
  // See https://stackoverflow.com/a/13274800/4447365
  child_id = static_cast<uint64_t *>(
      mmap(nullptr, sizeof(*child_id), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
  *child_id = 0;
  if (fork() == 0)
  {
    *child_id = Random::GenerateRandom64();
    exit(EXIT_SUCCESS);
  }
  else
  {
    wait(nullptr);
    auto parent_id     = Random::GenerateRandom64();
    auto child_id_copy = *child_id;
    munmap(static_cast<void *>(child_id), sizeof(*child_id));
    if (parent_id == child_id_copy)
    {
      std::cerr << "Child and parent ids are the same value " << parent_id << "\n";
      return -1;
    }
  }
  return 0;
}
#else
int main()
{
  return 0;
}
#endif
