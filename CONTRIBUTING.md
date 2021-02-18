# Contributing to opentelemetry-cpp

The OpenTelemetry C/C++ special interest group (SIG) meets regularly. See the
OpenTelemetry [community](https://github.com/open-telemetry/community#cc-sdk)
repo for information on this and other language SIGs.

See the [public meeting
notes](https://docs.google.com/document/d/1i1E4-_y4uJ083lCutKGDhkpi3n4_e774SBLi9hPLocw/edit)
for a summary description of past meetings. To request edit access, join the
meeting or get in touch on
[Slack](https://cloud-native.slack.com/archives/C01N3AT62SJ).

See the [community membership
document](https://github.com/open-telemetry/community/blob/main/community-membership.md)
on how to become a
[**Member**](https://github.com/open-telemetry/community/blob/main/community-membership.md#member),
[**Approver**](https://github.com/open-telemetry/community/blob/main/community-membership.md#approver)
and
[**Maintainer**](https://github.com/open-telemetry/community/blob/main/community-membership.md#maintainer).

## Development

OpenTelemetry C++ uses the [Google naming
convention](https://google.github.io/styleguide/cppguide.html#Naming).

Code is formatted automatically and enforced by CI.

### Build and Run Code Examples

Note: these instructions apply to examples configured with Bazel, see
example-specific documentation for other build automation tools.

Install the latest bazel version by following the steps listed
[here](https://docs.bazel.build/versions/master/install.html).

Select an example of interest from the [examples
folder](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/examples).
Inside each example directory is a `BUILD` file containing instructions for
Bazel. Find the binary name of your example by inspecting the contents of this
`BUILD` file.

Build the example from the root of the opentelemetry-cpp directory using Bazel.
Replace `<binary name>` with the identifier found in the previous step:

```sh
bazel build //examples/<example directory name>:<binary name>
```

Run the resulting executable to see telemetry from the application as it calls
the instrumented library: </li>

```sh
bazel-bin/examples/<example directory name>/<binary name>
```

For instance, building and running the `simple` example can be done as follows:

```sh
bazel build //examples/simple:example_simple
bazel-bin/examples/simple/example_simple
```

## Pull Requests

### How to Send Pull Requests

Everyone is welcome to contribute code to `opentelemetry-cpp` via GitHub pull
requests (PRs).

To create a new PR, fork the project in GitHub and clone the upstream repo:

```sh
git clone --recursive https://github.com/open-telemetry/opentelemetry-cpp.git
```

Add your fork as a remote:

```sh
git remote add fork https://github.com/YOUR_GITHUB_USERNAME/opentelemetry-cpp.git
```

If you haven't, make sure you are loading the submodules required to build
OpenTelemetry

```sh
git submodule init
git submodule update
```

Check out a new branch, make modifications and push the branch to your fork:

```sh
git checkout -b feature
# edit files
tools/format.sh
git commit
git push fork feature
```

Open a pull request against the main `opentelemetry-cpp` repo.

To run tests locally, please read the [CI instructions](ci/README.md).

### How to Receive Comments

* If the PR is not ready for review, please put `[WIP]` in the title, tag it as
  `work-in-progress`, or mark it as
  [`draft`](https://github.blog/2019-02-14-introducing-draft-pull-requests/).
* Make sure [CLA](https://identity.linuxfoundation.org/projects/cncf) is signed
  and CI is clear.
* For non-trivial changes, please update the [CHANGELOG](./CHANGELOG.md).

### How to Get PRs Merged

A PR is considered to be **ready to merge** when:

* It has received two approvals with at least one approval from
  [Approver](https://github.com/open-telemetry/community/blob/main/community-membership.md#approver)
  /
  [Maintainer](https://github.com/open-telemetry/community/blob/main/community-membership.md#maintainer)
  (at different company).
* A pull reqeust opened by an Approver / Maintainer can be merged with only one
  approval from  Approver / Maintainer (at different company).
* Major feedback items/points are resolved.
* It has been open for review for at least one working day. This gives people
  reasonable time to review.
* Trivial changes (typo, cosmetic, doc, etc.) don't have to wait for one day.
* Urgent fixes can take exceptions as long as it has been actively communicated.

Any Maintainer can merge the PR once it is **ready to merge**. Maintainer can
make conscious judgement to merge pull requests which have not strictly met
above mentioned requirements.

If a PR has been stuck (e.g. there are lots of debates and people couldn't agree
on each other), the owner should try to get people aligned by:

* Consolidating the perspectives and putting a summary in the PR. It is
  recommended to add a link into the PR description, which points to a comment
  with a summary in the PR conversation
* Stepping back to see if it makes sense to narrow down the scope of the PR or
  split it up.

If none of the above worked and the PR has been stuck for more than 2 weeks, the
owner should bring it to the [OpenTelemetry C++ SIG
meeting](https://zoom.us/j/8203130519). The meeting passcode is _77777_.

## Useful Resources

Hi! If you’re looking at this document, these resources will provide you the
knowledge to get started as a newcomer to the OpenTelemetry project. They will
help you understand the OpenTelemetry Project, its components, and specifically
the C++ repository.

### Reading Resources

* Medium
  [article](https://medium.com/opentelemetry/how-to-start-contributing-to-opentelemetry-b23991ad91f4)
  (October 2019) on how to start contributing to the OpenTelemetry project.
* Medium
  [article](https://medium.com/opentelemetry/opentelemetry-beyond-getting-started-5ac43cd0fe26)
  (January 2020) describing the overarching goals and use cases for
  OpenTelemetry.

### Relevant Documentation

* [OpenTelemetry
  Specification](https://github.com/open-telemetry/opentelemetry-specification)
  * The OpenTelemetry Specification describes the requirements and expectations
    of for all OpenTelemetry implementations.

* Read through the [OpenTelemetry
  Collector](https://github.com/open-telemetry/opentelemetry-collector) GitHub
  repository.
  * This repository has a lot of good information surrounding the OpenTelemetry
    ecosystem. At the top of the
    **[readme](https://github.com/open-telemetry/opentelemetry-collector/blob/main/README.md)**,
    there are multiple links that give newcomers a good idea of what the project
    is about and how to get involved in it.
* Read through the OpenTelemetry Python documentation
  * The
    [API](https://opentelemetry-python.readthedocs.io/en/stable/api/api.html)
    and
    [SDK](https://opentelemetry-python.readthedocs.io/en/stable/sdk/sdk.html)
    documentation provides a lot of information on what the classes and their
    functions are used for. Since there is currently minimal documentation for
    C++, use the Python repository’s extensive documentation to learn more about
    how the API and SDK work.

### Code Examples

* Follow the [simple trace
  example](https://github.com/open-telemetry/opentelemetry-cpp/pull/92) for an
  introduction to basic OpenTelemetry functionality in C++. Currently the
  example can be found in [PR
  #94](https://github.com/open-telemetry/opentelemetry-cpp/pull/94).

* Read through the [Java Quick-Start
  Guide](https://github.com/open-telemetry/opentelemetry-java/blob/main/QUICKSTART.md).
  This shows you how the classes and functions will interact in simple and easy
  to digest examples.
* Take a look at this [Java SDK
  example](https://github.com/open-telemetry/opentelemetry-java/tree/main/examples/sdk-usage).
  This shows a good use case of the SDK using stdout exporter.
* Take a look at the [Java Jaeger
  example](https://github.com/open-telemetry/opentelemetry-java/tree/main/examples/jaeger).
  This provides a brief introduction to the Jaeger exporter, its interface, and
  how to interact with the service.

Please contribute! You’re welcome to add more information if you come across any
helpful resources.
