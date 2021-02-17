# Notes on Abseil Variant implementation

This is a snapshot of Abseil Variant `absl::variant` from Abseil
`v2020-03-03#8`.

This code is required to compile OpenTelemetry code in vs2015 because MPark
Variant implementation is not compatible with vs2015.

Build option `HAVE_ABSEIL_VARIANT` allows to enable the build with
`absl::variant`, `absl::get` and `absl::visit` as defalt implementation for
`nostd::` classes.

Going forward it makes sense to use `absl::variant` as default implementation
for Windows OS and vs2015, vs2017, vs2019 and newer compilers.
