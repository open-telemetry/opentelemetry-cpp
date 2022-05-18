# OpenTelemetry C++

[![Slack](https://img.shields.io/badge/slack-@cncf/otel/cpp-brightgreen.svg?logo=slack)](https://cloud-native.slack.com/archives/C01N3AT62SJ)
[![codecov.io](https://codecov.io/gh/open-telemetry/opentelemetry-cpp/branch/main/graphs/badge.svg?)](https://codecov.io/gh/open-telemetry/opentelemetry-cpp/)
[![Build
Status](https://github.com/open-telemetry/opentelemetry-cpp/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/open-telemetry/opentelemetry-cpp/actions)
[![Release](https://img.shields.io/github/v/release/open-telemetry/opentelemetry-cpp?include_prereleases&style=)](https://github.com/open-telemetry/opentelemetry-cpp/releases/)

The C++ [OpenTelemetry](https://opentelemetry.io/) client.

## Project Status

| Signal  | Status                 | Project                                                                  |
| ------- | ---------------------- | ------------------------------------------------------------------------ |
| Traces  | Public Release         | N/A                                                                      |
| Metrics | Alpha        [1]       | N/A                                                                      |
| Logs    | Experimental [2]       | N/A                                                                      |

* [1]: The metric collection pipeline is available for testing purpose. Not
      ready for production. The earlier implementation (based on old
      specification) can be included in build by setting
      `ENABLE_METRICS_PREVIEW` preprocessor macro, and is included under
      `*/_metrics/*` directory. This would be eventually removed once the
      ongoing implemetation is stable.
* [2]: The current Log Signal Implementation is Experimental, and will change as
      the current OpenTelemetry Log specification matures. The current
      implementation can be included in build by setting `ENABLE_LOGS_PREVIEW`
      preprocessor macro.

## OpenTelemetry Specification Compatibility Matrix

| API Version | Core Version | Contrib Version         |
| ----------- |--------------|-------------------------|
| 1.0.0       | 1.0.0-rc4    | N/A [1]                 |
| 1.0.0       | 1.0.0        | N/A                     |

* [1]: We don't have releases for opentelemetry-cpp contrib repo. This may
      change in future.

## Supported C++ Versions

Code shipped from this repository generally supports the following versions of
C++ standards:

* ISO/IEC 14882:2011 (C++11, C++0x)
* ISO/IEC 14882:2014 (C++14, C++1y)
* ISO/IEC 14882:2017 (C++17, C++1z)
* ISO/IEC 14882:2020 (C++20)

Any exceptions to this are noted in the individual `README.md` files.

Please note that supporting the [C Programming
Language](https://en.wikipedia.org/wiki/C_(programming_language)) is not a goal
of the current project.

## Supported Development Platforms

 Our CI pipeline builds and tests on following `x86-64` platforms:

| Platform                                                            |   Build type  |
|---------------------------------------------------------------------|---------------|
| ubuntu-20.04 (Default GCC Compiler - 9.3.0)                         | CMake, Bazel  |
| ubuntu-18.04 (GCC 4.8 with -std=c++11 flag)                         | CMake [1]     |
| ubuntu-20.04 (Default GCC Compiler - 9.3.0 with -std=c++20 flags)   | CMake, Bazel  |
| macOS 10.15 (Xcode 12.2)                                            | Bazel         |
| Windows Server 2019 (Visual Studio Enterprise 2019)                 | CMake, Bazel  |

[1]: Bazel build is disabled for GCC 4.8, as gRPC library 1.38 and above
  (required by OTLP expoter) don't build with this compiler. See gRPC [official
  support](https://grpc.io/docs/#official-support) document. CMake build doesn't
  build OTLP exporter with GCC 4.8.

In general, the code shipped from this repository should build on all platforms
having C++ compiler with [supported C++ standards](#supported-c-versions).

## Dependencies

Please refer to [Dependencies.md](docs/dependencies.md) for OSS Dependencies and
license requirements.

## Installation

Please refer to [INSTALL.md](./INSTALL.md).

## Getting Started

As an application owner or the library author, you can find the getting started
guide and reference documentation on
[opentelemetry-cpp.readthedocs.io](https://opentelemetry-cpp.readthedocs.io/en/latest/)

The `examples/simple` directory contains a minimal program demonstrating how to
instrument a small library using a simple `processor` and console `exporter`,
along with build files for CMake and Bazel.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md)

We meet weekly, and the time of the meeting alternates between Monday at 13:00
PT and Wednesday at 10:00 PT. The meeting is subject to change depending on
contributors' availability. Check the [OpenTelemetry community
calendar](https://calendar.google.com/calendar/embed?src=google.com_b79e3e90j7bbsa2n2p5an5lf60%40group.calendar.google.com)
for specific dates and Zoom meeting links.

Meeting notes are available as a public [Google
doc](https://docs.google.com/document/d/1i1E4-_y4uJ083lCutKGDhkpi3n4_e774SBLi9hPLocw/edit?usp=sharing).
For edit access, get in touch on
[Slack](https://cloud-native.slack.com/archives/C01N3AT62SJ).

[Maintainers](https://github.com/open-telemetry/community/blob/main/community-membership.md#maintainer)
([@open-telemetry/cpp-maintainers](https://github.com/orgs/open-telemetry/teams/cpp-maintainers)):

* [Ehsan Saei](https://github.com/esigo)
* [Lalit Kumar Bhasin](https://github.com/lalitb), Microsoft
* [Tom Tan](https://github.com/ThomsonTan), Microsoft

[Approvers](https://github.com/open-telemetry/community/blob/main/community-membership.md#approver)
([@open-telemetry/cpp-approvers](https://github.com/orgs/open-telemetry/teams/cpp-approvers)):

* [Josh Suereth](https://github.com/jsuereth), Google
* [Reiley Yang](https://github.com/reyang), Microsoft
* [WenTao Ou](https://github.com/owent), Tencent

[Emeritus
Maintainer/Approver/Triager](https://github.com/open-telemetry/community/blob/main/community-membership.md#emeritus-maintainerapprovertriager):

* [Alolita Sharma](https://github.com/alolita)
* [Emil Mikulic](https://github.com/g-easy)
* [Jodee Varney](https://github.com/jodeev)
* [Johannes Tax](https://github.com/pyohannes)
* [Max Golovanov](https://github.com/maxgolov)
* [Ryan Burn](https://github.com/rnburn)

### Thanks to all the people who have contributed

[![contributors](https://contributors-img.web.app/image?repo=open-telemetry/opentelemetry-cpp)](https://github.com/open-telemetry/opentelemetry-cpp/graphs/contributors)

## Release Schedule

Refer to [project status](#project-status) for current status of the project.

See the [release
notes](https://github.com/open-telemetry/opentelemetry-cpp/releases) for
existing releases.

See the [project
milestones](https://github.com/open-telemetry/opentelemetry-cpp/milestones) for
details on upcoming releases. The dates and features described in issues and
milestones are estimates, and subject to change.
