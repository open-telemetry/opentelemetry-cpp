# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

load(
    "@rules_cc//cc:defs.bzl",
    rules_cc_binary = "cc_binary",
    rules_cc_import = "cc_import",
    rules_cc_library = "cc_library",
    rules_cc_test = "cc_test",
)


def _is_windows_impl(ctx):
  if ctx.target_platform_has_constraint(ctx.attr._windows_constraint[platform_common.ConstraintValueInfo]):
    return [DefaultInfo()]
  else:
    return [DefaultInfo()]
 
is_windows = rule(
  implementation = _is_windows_impl,
  attrs = {
    '_windows_constraint': attr.label(default = '@platforms//os:windows'),
  },
)


def otel_cc_library(**kwargs):
    # dump = kwargs["name"] + ": "
    # dump = dump + " mod=" + native.module_name()
    # dump = dump + " ver=" + native.module_version()
    # dump = dump + " pkg=" + native.package_name()
    # dump = dump + " repo=" + native.repo_name()
    # print(dump)

    # Link as .o (.obj) files, not .a (.lib)
    kwargs["alwayslink"] = kwargs.get("alwayslink", True)

    # Don't create .so files for dbg/fastbuild, always create static libs
    kwargs["linkstatic"] = kwargs.get("linkstatic", True)

#    x = is_windows(name=kwargs["name"]+"_is_windows")
#    print(x)

    rules_cc_library(**kwargs)

def otel_cc_binary(**kwargs):
    rules_cc_binary(**kwargs)

def otel_cc_test(**kwargs):
    rules_cc_test(**kwargs)

def otel_cc_import(**kwargs):
    rules_cc_import(**kwargs)

def otel_cc_shared_library(**kwargs):
    native.cc_shared_library(**kwargs)
