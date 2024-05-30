load("@depend_on_what_you_use//:defs.bzl", "dwyu_aspect_factory")

# https://github.com/martis42/depend_on_what_you_use?tab=readme-ov-file#use-dwyu
# bazel build ... --aspects=//bazel:dwyu.bzl%aspect --output_groups=dwyu

# Provide no arguments for the default behavior
# Or set a custom value for the various attributes
aspect = dwyu_aspect_factory()
