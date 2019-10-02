# Contributing to opentelemetry-cpp

The OpenTelemetry C/C++ special interest group (SIG) meets regularly. See the
OpenTelemetry [community](https://github.com/open-telemetry/community#cc-sdk)
repo for information on this and other language SIGs.

See the [public meeting notes](https://docs.google.com/document/d/1i1E4-_y4uJ083lCutKGDhkpi3n4_e774SBLi9hPLocw/edit)
for a summary description of past meetings. To request edit access, join the
meeting or get in touch on [Gitter](https://gitter.im/open-telemetry/opentelemetry-cpp).

See to the [community membership document](https://github.com/open-telemetry/community/blob/master/community-membership.md)
on how to become a [**Member**](https://github.com/open-telemetry/community/blob/master/community-membership.md#member),
[**Approver**](https://github.com/open-telemetry/community/blob/master/community-membership.md#approver)
and [**Maintainer**](https://github.com/open-telemetry/community/blob/master/community-membership.md#maintainer).

## Development

TBD

## Pull Requests

### How to Send Pull Requests

Everyone is welcome to contribute code to `opentelemetry-cpp` via GitHub pull
requests (PRs).

To create a new PR, fork the project in GitHub and clone the upstream repo:

```sh
$ git clone https://github.com/open-telemetry/opentelemetry-cpp.git
```

Add your fork as an origin:

```sh
$ git remote add fork https://github.com/YOUR_GITHUB_USERNAME/opentelemetry-cpp.git
```

Build:

TBD

Test:

TBD

Check out a new branch, make modifications and push the branch to your fork:

```sh
$ git checkout -b feature
# edit files
$ git commit
$ git push fork feature
```

Open a pull request against the main `opentelemetry-cpp` repo.

### How to Receive Comments

* If the PR is not ready for review, please put `[WIP]` in the title, tag it
  as `work-in-progress`, or mark it as [`draft`](https://github.blog/2019-02-14-introducing-draft-pull-requests/).
* Make sure CLA is signed and CI is clear.

### How to Get PRs Merged

A PR is considered to be **ready to merge** when:
* It has received two approvals from [Approvers](https://github.com/open-telemetry/community/blob/master/community-membership.md#approver)
  / [Maintainers](https://github.com/open-telemetry/community/blob/master/community-membership.md#maintainer)
  (at different companies).
* Major feedbacks are resolved.
* It has been open for review for at least one working day. This gives people
  reasonable time to review.
* Trivial change (typo, cosmetic, doc, etc.) doesn't have to wait for one day.
* Urgent fix can take exception as long as it has been actively communicated.

Any Approver / Maintainer can merge the PR once it is **ready to merge**.

## Style Guide

TBD
