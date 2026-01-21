# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

import argparse
import re
import sys
from collections import defaultdict
from enum import Enum
from pathlib import Path
from typing import Dict, List, NamedTuple, Optional, Set

# --- Configuration ---
REPO_NAME = "opentelemetry-cpp"
MAX_ROWS = 1000
WARNING_RE = re.compile(
    r"^(?P<file>.+):(?P<line>\d+):(?P<col>\d+): warning: (?P<msg>.+) "
    r"\[(?P<check>.+)\]$"
)
ANSI_RE = re.compile(r"\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")


class OutputKeys(str, Enum):
    TOTAL_WARNINGS = "TOTAL_WARNINGS"
    REPORT_PATH = "REPORT_PATH"


class ClangTidyWarning(NamedTuple):
    file: str
    line: int
    col: int
    msg: str
    check: str


def clean_path(path: str) -> str:
    """Strip path prefix to make it relative to the repo or CWD."""
    if f"{REPO_NAME}/" in path:
        return path.split(f"{REPO_NAME}/", 1)[1]
    try:
        return str(Path(path).relative_to(Path.cwd()))
    except ValueError:
        return path


def parse_log(log_path: Path) -> Set[ClangTidyWarning]:
    if not log_path.exists():
        sys.exit(f"[ERROR] Log not found: {log_path}")
    unique = set()
    with log_path.open("r", encoding="utf-8", errors="replace") as f:
        for line in f:
            line = ANSI_RE.sub("", line.strip())
            if "warning:" not in line:
                continue
            match = WARNING_RE.match(line)
            if match:
                unique.add(
                    ClangTidyWarning(
                        clean_path(match.group("file")),
                        int(match.group("line")),
                        int(match.group("col")),
                        match.group("msg"),
                        match.group("check"),
                    )
                )
    return unique


def generate_report(
    warnings: Set[ClangTidyWarning],
    output_path: Path,
):
    by_check: Dict[str, List[ClangTidyWarning]] = defaultdict(list)
    by_file: Dict[str, List[ClangTidyWarning]] = defaultdict(list)

    for w in warnings:
        by_check[w.check].append(w)
        by_file[w.file].append(w)

    with output_path.open("w", encoding="utf-8") as md:
        title = "#### "
        md.write(
            f"{title} `clang-tidy` job reported {len(warnings)} warnings\n\n"
        )

        if not warnings:
            return

        def write_section(
            title,
            data,
            item_sort_key,
            header,
            row_fmt,
            group_key,
            reverse,
            summary_col_name,
        ):
            md.write(f"<details><summary><b>{title}</b><i> - Click to expand </i></summary>\n\n")
            sorted_groups = sorted(
                data.items(), key=group_key, reverse=reverse
            )

            # Summary Table (Sorted by Count Descending)
            summary_groups = sorted(
                data.items(), key=lambda x: len(x[1]), reverse=True
            )
            md.write("#### Summary\n\n")
            md.write(f"| {summary_col_name} | Count |\n|---|---|\n")
            for key, items in summary_groups:
                md.write(f"| {key} | {len(items)} |\n")
            md.write("\n")

            md.write("#### Details\n\n")
            for key, items in sorted_groups:
                md.write(
                    f"\n----\n\n**{key}** ({len(items)} warnings)\n\n{header}\n"
                )
                for i, w in enumerate(sorted(items, key=item_sort_key)):
                    if i >= MAX_ROWS:
                        remaining = len(items) - i
                        md.write(
                            f"| ... | ... | *{remaining} more omitted...* |\n"
                        )
                        break
                    md.write(row_fmt(w) + "\n")
                md.write("\n")
            md.write("</details>\n\n")

        # Warnings by File: Sorted Alphabetically
        write_section(
            "Warnings by File",
            by_file,
            item_sort_key=lambda w: w.line,
            header="| Line | Check | Message |\n|---|---|---|",
            row_fmt=lambda w: f"| {w.line} | `{w.check}` | {w.msg} |",
            group_key=lambda x: x[0],
            reverse=False,
            summary_col_name="File",
        )

        # Warnings by clang-tidy check: Sort by Warning count
        write_section(
            "Warnings by clang-tidy Check",
            by_check,
            item_sort_key=lambda w: (w.file, w.line),
            header="| File | Line | Message |\n|---|---|---|",
            row_fmt=lambda w: f"| `{w.file}` | {w.line} | {w.msg} |",
            group_key=lambda x: len(x[1]),
            reverse=True,
            summary_col_name="Check",
        )

        md.write("\n----\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-l",
        "--build_log",
        type=Path,
        required=True,
        help="Clang-tidy log file",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default="clang_tidy_report.md",
        help="Output report path",
    )
    args = parser.parse_args()

    warnings = parse_log(args.build_log)
    generate_report(warnings, args.output)

    sys.stdout.write(f"{OutputKeys.TOTAL_WARNINGS.value}={len(warnings)}\n")
    if args.output.exists():
        sys.stdout.write(f"{OutputKeys.REPORT_PATH.value}={args.output.resolve()}\n")
    else:
        sys.exit(f"[ERROR] Failed to write report: {args.output.resolve()}")

if __name__ == "__main__":
    main()