# Release Process

## Pre Release

1. Make sure all relevant changes for this release are included under `Unreleased` section in `CHANGELOG.md` and are in language that non-contributors to the project can understand.

2. Run the pre-release script. It creates a branch `pre_release_<new-tag>` and updates `CHANGELOG.md` with the `<new-tag>`:
    ```
    ./pre_release.sh -t <new-tag>
    ```
3. Verify that CHANGELOG.md is updated properly:
    ```
    git diff master
    ```
4. Push the changes to upstream and create a Pull Request on GitHub.
    Be sure to include the curated changes from the [Changelog](./CHANGELOG.md) in the description.

## Tag

Once the above Pull Request has been approved and merged it is time to tag the merged commit.

***IMPORTANT***: It is critical you use the same tag that you used in the Pre-Release step!
Failure to do so will leave things in a broken state.

1. Note down the commit hash of the master branch after above PR request is merged : <commit-hash>
    ```
     git show -s --format=%H
    ```
2. Create a github tag on this commit hash:
    ```
    git tag -a "<new-tag>" -s -m "Version <new-tag>" "<commit-hash>"

3. Push tag to upstream remote
    ```
    git push upstream
    ```

## Release
Finally create a Release for the new <new-tag> on GitHub. The release body should include all the release notes from the Changelog for this release.
