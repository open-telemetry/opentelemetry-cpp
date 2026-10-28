#!/usr/bin/env python3

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

"""Validate OpenTelemetry YAML configuration files against the JSON schema."""

import argparse
import glob
import json
import os
import sys
from typing import Any

import jsonschema
import yaml


# Files with this marker in a YAML comment are intentionally invalid and skipped.
_SKIP_MARKER = "SKIP_SCHEMA_VALIDATION"


class _VersionError(Exception):
    pass


def _load_schema(path: str) -> dict[str, Any]:
    try:
        with open(path, encoding="utf-8") as f:
            schema = json.load(f)
    except FileNotFoundError:
        print(f"ERROR: schema file not found: {path}", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"ERROR: invalid JSON in schema file: {e}", file=sys.stderr)
        sys.exit(1)
    try:
        jsonschema.Draft202012Validator.check_schema(schema)
    except jsonschema.exceptions.SchemaError as e:
        print(f"ERROR: schema is not valid JSON Schema: {e.message}", file=sys.stderr)
        sys.exit(1)
    return schema

def _parse_version(ver: str) -> tuple[int, int] | None:
    """Return (major, minor) from a version string like 'v1.1.0' or '1.0'."""
    try:
        major, minor, *_ = ver.lstrip("v").split(".")
        return (int(major), int(minor))
    except ValueError:
        return None


def _validate_file(
    path: str,
    schema: dict[str, Any],
    schema_version_tuple: tuple[int, int] | None = None,
) -> str | list[jsonschema.exceptions.ValidationError]:
    """Return a skip-reason string, or a list of jsonschema errors (empty on success)."""
    with open(path, encoding="utf-8") as f:
        raw = f.read()
    comment = next(
        (line for line in raw.splitlines() if line.lstrip().startswith("#") and _SKIP_MARKER in line),
        None,
    )
    if comment is not None:
        reason = comment.split(_SKIP_MARKER, 1)[1].lstrip(": ").strip()
        return reason or "marked as skip"
    doc = yaml.safe_load(raw)
    if not isinstance(doc, dict) or not isinstance(doc.get("file_format"), str):
        return "no valid file_format"
    if schema_version_tuple is not None:
        file_ver = _parse_version(doc["file_format"])
        if file_ver is not None and file_ver > schema_version_tuple:
            raise _VersionError(
                f"file_format {doc['file_format']} is newer than pinned schema "
                f"{'.'.join(str(x) for x in schema_version_tuple)} — update third_party_release"
            )
    return list(jsonschema.Draft202012Validator(schema).iter_errors(doc))

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "files",
        nargs="*",
        help="YAML files to validate (defaults to examples/configuration/*.yaml "
             "and functional/configuration/shelltests/*.yaml)",
    )
    parser.add_argument(
        "--schema",
        required=True,
        metavar="PATH",
        help="Path to opentelemetry_configuration.json (from "
             "https://github.com/open-telemetry/opentelemetry-configuration)",
    )
    parser.add_argument(
        "--schema-version",
        metavar="VERSION",
        help="Schema version for display and version-mismatch checks (e.g. v1.1.0)",
    )
    args = parser.parse_args()
    schema = _load_schema(args.schema)
    schema_version_tuple = _parse_version(args.schema_version) if args.schema_version else None

    print(f"Schema: {args.schema_version or args.schema}\n")

    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    files: list[str] = args.files or sorted(
        glob.glob(os.path.join(repo_root, "examples", "configuration", "*.yaml"))
        + glob.glob(os.path.join(repo_root, "functional", "configuration", "shelltests", "*.yaml"))
    )
    if not files:
        print("ERROR: no YAML files found to validate.", file=sys.stderr)
        sys.exit(1)

    def rel(path: str) -> str:
        return os.path.relpath(path, repo_root)

    skipped: list[str] = []
    failed: list[str] = []

    def _fail(path: str, tag: str, detail: object) -> None:
        failed.append(path)
        print(f"FAIL: {rel(path)}")
        print(f"  [{tag}] {detail}")

    for path in files:
        try:
            result = _validate_file(path, schema, schema_version_tuple)
        except _VersionError as e:
            _fail(path, "version", e); continue
        except yaml.YAMLError as e:
            _fail(path, "yaml", e); continue
        except OSError as e:
            _fail(path, "error", e); continue
        if isinstance(result, str):
            skipped.append(path)
            print(f"SKIP: {rel(path)}")
            print(f"      {result}")
        elif result:
            failed.append(path)
            print(f"FAIL: {rel(path)}")
            for err in result:
                loc = " > ".join(str(p) for p in err.absolute_path) or "(root)"
                print(f"  [{loc}] {err.message}")
        else:
            print(f"OK:   {rel(path)}")

    if failed:
        print(f"\n{len(failed)} file(s) failed validation.")
        sys.exit(1)

    validated = len(files) - len(skipped)
    print(f"\nAll {validated} file(s) passed validation ({len(skipped)} skipped).")


if __name__ == "__main__":
    main()
