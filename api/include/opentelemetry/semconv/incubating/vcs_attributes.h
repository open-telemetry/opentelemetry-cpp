/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_attributes-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace vcs
{

/**
 * The ID of the change (pull request/merge request/changelist) if applicable. This is usually a
 * unique (within repository) identifier generated by the VCS system.
 */
static constexpr const char *kVcsChangeId = "vcs.change.id";

/**
 * The state of the change (pull request/merge request/changelist).
 */
static constexpr const char *kVcsChangeState = "vcs.change.state";

/**
 * The human readable title of the change (pull request/merge request/changelist). This title is
 * often a brief summary of the change and may get merged in to a ref as the commit summary.
 */
static constexpr const char *kVcsChangeTitle = "vcs.change.title";

/**
 * The type of line change being measured on a branch or change.
 */
static constexpr const char *kVcsLineChangeType = "vcs.line_change.type";

/**
 * The group owner within the version control system.
 */
static constexpr const char *kVcsOwnerName = "vcs.owner.name";

/**
 * The name of the version control system provider.
 */
static constexpr const char *kVcsProviderName = "vcs.provider.name";

/**
 * The name of the <a href="https://git-scm.com/docs/gitglossary#def_ref">reference</a> such as
 * <strong>branch</strong> or <strong>tag</strong> in the repository. <p>
 * @code base @endcode refers to the starting point of a change. For example, @code main @endcode
 * would be the base reference of type branch if you've created a new
 * reference of type branch from it and created new commits.
 */
static constexpr const char *kVcsRefBaseName = "vcs.ref.base.name";

/**
 * The revision, literally <a href="https://www.merriam-webster.com/dictionary/revision">revised
 * version</a>, The revision most often refers to a commit object in Git, or a revision number in
 * SVN. <p>
 * @code base @endcode refers to the starting point of a change. For example, @code main @endcode
 * would be the base reference of type branch if you've created a new
 * reference of type branch from it and created new commits. The
 * revision can be a full <a href="https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-5.pdf">hash
 * value (see glossary)</a>, of the recorded change to a ref within a repository pointing to a
 * commit <a href="https://git-scm.com/docs/git-commit">commit</a> object. It does
 * not necessarily have to be a hash; it can simply define a <a
 * href="https://svnbook.red-bean.com/en/1.7/svn.tour.revs.specifiers.html">revision number</a>
 * which is an integer that is monotonically increasing. In cases where
 * it is identical to the @code ref.base.name @endcode, it SHOULD still be included.
 * It is up to the implementer to decide which value to set as the
 * revision based on the VCS system and situational context.
 */
static constexpr const char *kVcsRefBaseRevision = "vcs.ref.base.revision";

/**
 * The type of the <a href="https://git-scm.com/docs/gitglossary#def_ref">reference</a> in the
 * repository. <p>
 * @code base @endcode refers to the starting point of a change. For example, @code main @endcode
 * would be the base reference of type branch if you've created a new
 * reference of type branch from it and created new commits.
 */
static constexpr const char *kVcsRefBaseType = "vcs.ref.base.type";

/**
 * The name of the <a href="https://git-scm.com/docs/gitglossary#def_ref">reference</a> such as
 * <strong>branch</strong> or <strong>tag</strong> in the repository. <p>
 * @code head @endcode refers to where you are right now; the current reference at a
 * given time.
 */
static constexpr const char *kVcsRefHeadName = "vcs.ref.head.name";

/**
 * The revision, literally <a href="https://www.merriam-webster.com/dictionary/revision">revised
 * version</a>, The revision most often refers to a commit object in Git, or a revision number in
 * SVN. <p>
 * @code head @endcode refers to where you are right now; the current reference at a
 * given time.The revision can be a full <a
 * href="https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-5.pdf">hash value (see glossary)</a>,
 * of the recorded change to a ref within a repository pointing to a
 * commit <a href="https://git-scm.com/docs/git-commit">commit</a> object. It does
 * not necessarily have to be a hash; it can simply define a <a
 * href="https://svnbook.red-bean.com/en/1.7/svn.tour.revs.specifiers.html">revision number</a>
 * which is an integer that is monotonically increasing. In cases where
 * it is identical to the @code ref.head.name @endcode, it SHOULD still be included.
 * It is up to the implementer to decide which value to set as the
 * revision based on the VCS system and situational context.
 */
static constexpr const char *kVcsRefHeadRevision = "vcs.ref.head.revision";

/**
 * The type of the <a href="https://git-scm.com/docs/gitglossary#def_ref">reference</a> in the
 * repository. <p>
 * @code head @endcode refers to where you are right now; the current reference at a
 * given time.
 */
static constexpr const char *kVcsRefHeadType = "vcs.ref.head.type";

/**
 * The type of the <a href="https://git-scm.com/docs/gitglossary#def_ref">reference</a> in the
 * repository.
 */
static constexpr const char *kVcsRefType = "vcs.ref.type";

/**
 * Deprecated, use @code vcs.change.id @endcode instead.
 *
 * @deprecated
 * {"note": "Deprecated, use @code vcs.change.id @endcode instead.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kVcsRepositoryChangeId =
    "vcs.repository.change.id";

/**
 * Deprecated, use @code vcs.change.title @endcode instead.
 *
 * @deprecated
 * {"note": "Deprecated, use @code vcs.change.title @endcode instead.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kVcsRepositoryChangeTitle =
    "vcs.repository.change.title";

/**
 * The human readable name of the repository. It SHOULD NOT include any additional identifier like
 * Group/SubGroup in GitLab or organization in GitHub. <p> Due to it only being the name, it can
 * clash with forks of the same repository if collecting telemetry across multiple orgs or groups in
 * the same backends.
 */
static constexpr const char *kVcsRepositoryName = "vcs.repository.name";

/**
 * Deprecated, use @code vcs.ref.head.name @endcode instead.
 *
 * @deprecated
 * {"note": "Deprecated, use @code vcs.ref.head.name @endcode instead.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kVcsRepositoryRefName =
    "vcs.repository.ref.name";

/**
 * Deprecated, use @code vcs.ref.head.revision @endcode instead.
 *
 * @deprecated
 * {"note": "Deprecated, use @code vcs.ref.head.revision @endcode instead.", "reason":
 * "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kVcsRepositoryRefRevision =
    "vcs.repository.ref.revision";

/**
 * Deprecated, use @code vcs.ref.head.type @endcode instead.
 *
 * @deprecated
 * {"note": "Deprecated, use @code vcs.ref.head.type @endcode instead.", "reason": "uncategorized"}
 */
OPENTELEMETRY_DEPRECATED static constexpr const char *kVcsRepositoryRefType =
    "vcs.repository.ref.type";

/**
 * The <a
 * href="https://support.google.com/webmasters/answer/10347851?hl=en#:~:text=A%20canonical%20URL%20is%20the,Google%20chooses%20one%20as%20canonical.">canonical
 * URL</a> of the repository providing the complete HTTP(S) address in order to locate and identify
 * the repository through a browser. <p> In Git Version Control Systems, the canonical URL SHOULD
 * NOT include the @code .git @endcode extension.
 */
static constexpr const char *kVcsRepositoryUrlFull = "vcs.repository.url.full";

/**
 * The type of revision comparison.
 */
static constexpr const char *kVcsRevisionDeltaDirection = "vcs.revision_delta.direction";

namespace VcsChangeStateValues
{
/**
 * Open means the change is currently active and under review. It hasn't been merged into the target
 * branch yet, and it's still possible to make changes or add comments.
 */
static constexpr const char *kOpen = "open";

/**
 * WIP (work-in-progress, draft) means the change is still in progress and not yet ready for a full
 * review. It might still undergo significant changes.
 */
static constexpr const char *kWip = "wip";

/**
 * Closed means the merge request has been closed without merging. This can happen for various
 * reasons, such as the changes being deemed unnecessary, the issue being resolved in another way,
 * or the author deciding to withdraw the request.
 */
static constexpr const char *kClosed = "closed";

/**
 * Merged indicates that the change has been successfully integrated into the target codebase.
 */
static constexpr const char *kMerged = "merged";

}  // namespace VcsChangeStateValues

namespace VcsLineChangeTypeValues
{
/**
 * How many lines were added.
 */
static constexpr const char *kAdded = "added";

/**
 * How many lines were removed.
 */
static constexpr const char *kRemoved = "removed";

}  // namespace VcsLineChangeTypeValues

namespace VcsProviderNameValues
{
/**
 * <a href="https://github.com">GitHub</a>
 */
static constexpr const char *kGithub = "github";

/**
 * <a href="https://gitlab.com">GitLab</a>
 */
static constexpr const char *kGitlab = "gitlab";

/**
 * <a href="https://gitea.io">Gitea</a>
 */
static constexpr const char *kGittea = "gittea";

/**
 * <a href="https://bitbucket.org">Bitbucket</a>
 */
static constexpr const char *kBitbucket = "bitbucket";

}  // namespace VcsProviderNameValues

namespace VcsRefBaseTypeValues
{
/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddefbranchabranch">branch</a>
 */
static constexpr const char *kBranch = "branch";

/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddeftagatag">tag</a>
 */
static constexpr const char *kTag = "tag";

}  // namespace VcsRefBaseTypeValues

namespace VcsRefHeadTypeValues
{
/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddefbranchabranch">branch</a>
 */
static constexpr const char *kBranch = "branch";

/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddeftagatag">tag</a>
 */
static constexpr const char *kTag = "tag";

}  // namespace VcsRefHeadTypeValues

namespace VcsRefTypeValues
{
/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddefbranchabranch">branch</a>
 */
static constexpr const char *kBranch = "branch";

/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddeftagatag">tag</a>
 */
static constexpr const char *kTag = "tag";

}  // namespace VcsRefTypeValues

namespace VcsRepositoryRefTypeValues
{
/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddefbranchabranch">branch</a>
 */
static constexpr const char *kBranch = "branch";

/**
 * <a
 * href="https://git-scm.com/docs/gitglossary#Documentation/gitglossary.txt-aiddeftagatag">tag</a>
 */
static constexpr const char *kTag = "tag";

}  // namespace VcsRepositoryRefTypeValues

namespace VcsRevisionDeltaDirectionValues
{
/**
 * How many revisions the change is behind the target ref.
 */
static constexpr const char *kBehind = "behind";

/**
 * How many revisions the change is ahead of the target ref.
 */
static constexpr const char *kAhead = "ahead";

}  // namespace VcsRevisionDeltaDirectionValues

}  // namespace vcs
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
