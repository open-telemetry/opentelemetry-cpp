# Copyright 2021, OpenTelemetry Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(WITH_OTLP)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()

if(WITH_ZIPKIN)
  add_subdirectory(zipkin)
endif()

if(WITH_ELASTICSEARCH AND WITH_LOGS_PREVIEW)
  add_subdirectory(elasticsearch)
endif()

if(WITH_ETW)
  add_subdirectory(etw)
endif()

if(WITH_JAEGER)
  add_subdirectory(jaeger)
endif()
