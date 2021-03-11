# OpenTelemetry C++

[![Slack](https://img.shields.io/badge/slack-@cncf/otel/cpp-brightgreen.svg?logo=slack)](https://cloud-native.slack.com/archives/C01N3AT62SJ)
[![codecov.io](https://codecov.io/gh/open-telemetry/opentelemetry-cpp/branch/main/graphs/badge.svg?)](https://codecov.io/gh/open-telemetry/opentelemetry-cpp/)
[![Build
Status](https://action-badges.now.sh/open-telemetry/opentelemetry-cpp)](https://github.com/open-telemetry/opentelemetry-cpp/actions)
[![Release](https://img.shields.io/github/v/release/open-telemetry/opentelemetry-cpp?include_prereleases&style=)](https://github.com/open-telemetry/opentelemetry-cpp/releases/)

The C++ [OpenTelemetry](https://opentelemetry.io/) client.

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

* ubuntu-18.04 (Default GCC Compiler - 7.5.0)
* ubuntu-18.04 (GCC 4.8 with -std=c++11 flag)
* ubuntu-20.04 (Default GCC Compiler - 9.3.0 with -std=c++20 flags)
* macOS 10.15 (Xcode 12.2)
* Windows Server 2019 (Visual Studio Enterprise 2019)

In general, the code shipped from this repository should build on all platforms
having C++ compiler with [supported C++ standards](#supported-c-versions).

## Installation

Please refer to [INSTALL.md](./INSTALL.md).

## Quick Start

The `examples/simple` directory contains a minimal program demonstrating how to
instrument a small library using a simple `processor` and console `exporter`,
along with build files for CMake and Bazel.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md)

We meet weekly, and the time of the meeting alternates between Monday at 15:00
PT and Wednesday at 10:00 PT. The meeting is subject to change depending on
contributors' availability. Check the [OpenTelemetry community
calendar](https://calendar.google.com/calendar/embed?src=google.com_b79e3e90j7bbsa2n2p5an5lf60%40group.calendar.google.com)
for specific dates.

Meetings take place via [Zoom video conference](https://zoom.us/j/8203130519).
The passcode is _77777_.

Meeting notes are available as a public [Google
doc](https://docs.google.com/document/d/1i1E4-_y4uJ083lCutKGDhkpi3n4_e774SBLi9hPLocw/edit?usp=sharing).
For edit access, get in touch on
[Slack](https://cloud-native.slack.com/archives/C01N3AT62SJ).

Approvers
([@open-telemetry/cpp-approvers](https://github.com/orgs/open-telemetry/teams/cpp-approvers)):

* [Johannes Tax](https://github.com/pyohannes), Microsoft
* [Josh Suereth](https://github.com/jsuereth), Google
* [Max Golovanov](https://github.com/maxgolov), Microsoft
* [Ryan Burn](https://github.com/rnburn), Lightstep
* [Tom Tan](https://github.com/ThomsonTan), Microsoft

*Find more about the approver role in [community
repository](https://github.com/open-telemetry/community/blob/main/community-membership.md#approver).*

Maintainers
([@open-telemetry/cpp-maintainers](https://github.com/orgs/open-telemetry/teams/cpp-maintainers)):

* [Emil Mikulic](https://github.com/g-easy), Google
* [Lalit Kumar Bhasin](https://github.com/lalitb), Microsoft
* [Reiley Yang](https://github.com/reyang), Microsoft

*Find more about the maintainer role in [community
repository](https://github.com/open-telemetry/community/blob/main/community-membership.md#maintainer).*

Triagers
([@open-telemetry/cpp-triagers](https://github.com/orgs/open-telemetry/teams/cpp-triagers)):

* [Alolita Sharma](https://github.com/alolita), Amazon
* [Jodee Varney](https://github.com/jodeev), New Relic

*Find more about the triager role in [community
repository](https://github.com/open-telemetry/community/blob/main/community-membership.md#triager).*

### Thanks to all the people who have contributed

[![contributors](https://contributors-img.web.app/image?repo=open-telemetry/opentelemetry-cpp)](https://github.com/open-telemetry/opentelemetry-cpp/graphs/contributors)

## Release Schedule

OpenTelemetry C++ is under active development.

The API and SDK packages are __not yet generally available__, and releases are
not guaranteed to conform to a specific version of the specification. Future
releases will not attempt to maintain backwards compatibility with previous
releases. Each alpha and beta release could include significant changes to the
API and SDK packages, making them incompatible with each other.

See the [release
notes](https://github.com/open-telemetry/opentelemetry-cpp/releases) for
existing releases.

See the [project
milestones](https://github.com/open-telemetry/opentelemetry-cpp/milestones) for
details on upcoming releases. The dates and features described in issues and
milestones are estimates, and subject to change.
