#!/usr/bin/env python3
"""Normalize C/C++ header guards in place.

Usage:
    fix_header_guards.py <project-root-or-header> [<project-root-or-header> ...]

Each directory argument is treated as a separate project root. Header guards are
computed from the header path relative to that root, not from the absolute
filesystem path. For example, running this from anywhere:

    fix_header_guards.py ~/code/oss/threading_library

will turn:

    ~/code/oss/threading_library/Library/Includes/DataStructures/Stack.hpp

into:

    _LIBRARY_DATASTRUCTURES_STACK_HPP

A direct header argument is also supported:

    fix_header_guards.py ~/code/oss/threading_library/Library/Includes/Stack.hpp

In that case the header's parent directory is the root, so the guard becomes:

    _STACK_HPP

Only .h and .hpp files are considered. The script rewrites existing guards when
it can find a normal #ifndef/#define pair, or adds a guard around files that do
not have one.

Do not pass a parent directory that contains several independent projects unless
you want guards to be relative to that parent. Pass each project root as its own
argument instead. For single-header usage, the header's parent directory is used
as the root.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


EXCLUDE_FOLDERS = {"include", "includes", "src", "source", "sources"}
HEADER_SUFFIXES = {".h", ".hpp"}
# Compile the guard-detection regexes once so batch runs do not rebuild them for
# every processed header.
IFNDEF_PATTERN = re.compile(r"^\s*#ifndef\s+(\S+)\s*$")
DEFINE_PATTERN = re.compile(r"^\s*#define\s+(\S+)\s*$")
ENDIF_PATTERN = re.compile(r"^\s*#endif\s*(//\s*!?\s*(\S+))?\s*$")


def compute_expected_guard(header_path: Path, project_root: Path) -> str:
    parts: list[str] = []
    # The root-relative path is the important part of the algorithm: it keeps
    # generated guards stable across machines and checkout locations.
    relative_header_path = header_path.resolve().relative_to(project_root.resolve())

    for parent in relative_header_path.parent.parts:
        # Layout-only folders are skipped so the guard reflects the library API
        # path rather than incidental directory structure.
        if parent.lower() not in EXCLUDE_FOLDERS:
            parts.append(parent)

    suffix = relative_header_path.suffix.lower()
    suffix_name = "H" if ".h" == suffix else "HPP"
    parts.append(relative_header_path.stem)

    return "_" + "_".join(part.upper() for part in parts) + "_" + suffix_name


def collect_header_files(paths: list[Path]) -> tuple[list[tuple[Path, Path]], list[str]]:
    # Key by resolved path so the same header can be reached through multiple
    # arguments without being rewritten twice in one run.
    header_files: dict[Path, tuple[Path, Path]] = {}
    errors: list[str] = []

    for path in paths:
        if not path.exists():
            errors.append(f"ERROR: Path '{path}' not found.")
            continue

        if path.is_file():
            if path.suffix.lower() in HEADER_SUFFIXES:
                # A direct file argument has no wider project context, so its
                # parent directory becomes the root for guard computation.
                header_files.setdefault(path.resolve(), (path, path.parent))
            continue

        for candidate in path.rglob("*"):
            if candidate.is_file() and candidate.suffix.lower() in HEADER_SUFFIXES:
                # A directory argument is the project root for every header
                # discovered under it.
                header_files.setdefault(candidate.resolve(), (candidate, path))

    # A deterministic order keeps tool output stable and avoids needless churn
    # when the script is used in CI or from a Make target.
    return sorted(header_files.values(), key=lambda header: header[0]), errors


def build_updated_content(source: str, expected_guard: str) -> str:
    lines = source.splitlines(keepends=True)

    ifndef_line: int | None = None
    define_line: int | None = None
    endif_line: int | None = None

    # Use the first #ifndef followed by the first later #define as the guard
    # opening. This intentionally avoids rewriting unrelated preprocessor uses
    # deeper in the file.
    for index, line in enumerate(lines):
        if None == ifndef_line:
            if IFNDEF_PATTERN.match(line):
                ifndef_line = index
                continue

        if None != ifndef_line and None == define_line:
            if DEFINE_PATTERN.match(line):
                define_line = index
                break

    # The guard closing is conventionally the last #endif in the file.
    for index in range(len(lines) - 1, -1, -1):
        if ENDIF_PATTERN.match(lines[index]):
            endif_line = index
            break

    inserted_prefix_lines = 0

    if None != ifndef_line and None != define_line and ifndef_line < define_line:
        lines[ifndef_line] = f"#ifndef {expected_guard}\n"
        lines[define_line] = f"#define {expected_guard}\n"
    else:
        # If a usable guard pair does not exist, prepend one instead of trying
        # to infer whether some other preprocessor lines should be rewritten.
        lines.insert(0, f"#ifndef {expected_guard}\n")
        lines.insert(1, f"#define {expected_guard}\n")
        inserted_prefix_lines = 2

    if None != endif_line:
        # When we inserted the guard pair above, the original trailing #endif
        # moves down by two lines. Adjust the index before rewriting it.
        lines[endif_line + inserted_prefix_lines] = f"#endif  // !{expected_guard}\n"
    else:
        if lines and not lines[-1].endswith("\n"):
            lines[-1] = f"{lines[-1]}\n"
        if lines and "\n" != lines[-1]:
            lines.append("\n")
        lines.append(f"#endif  // !{expected_guard}\n")

    return "".join(lines)


def fix_header_guard(header_path: Path, project_root: Path) -> tuple[bool, bool]:
    try:
        original_content = header_path.read_text(encoding="utf-8")
    except OSError as error:
        print(f"ERROR: Failed to read '{header_path}': {error}", file=sys.stderr)
        return False, False

    expected_guard = compute_expected_guard(header_path, project_root)
    updated_content = build_updated_content(original_content, expected_guard)

    # Skip writes for unchanged files so normal builds do not perturb mtimes and
    # incremental rebuilds stay quiet.
    if updated_content == original_content:
        return False, True

    try:
        header_path.write_text(updated_content, encoding="utf-8")
    except OSError as error:
        print(f"ERROR: Failed to write '{header_path}': {error}", file=sys.stderr)
        return False, False

    print(f"FIXED: Header guard updated in {header_path}")
    return True, True


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print("Usage: fix_header_guards.py <path> [<path> ...]", file=sys.stderr)
        return 1

    input_paths = [Path(argument) for argument in argv[1:]]
    header_files, errors = collect_header_files(input_paths)

    for error in errors:
        print(error, file=sys.stderr)

    changed_count = 0
    success = not errors

    for header_path, project_root in header_files:
        changed, processed = fix_header_guard(header_path, project_root)
        changed_count += int(changed)
        success = success and processed

    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
