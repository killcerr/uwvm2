#!/usr/bin/env python3
"""
Detect incorrect includes inserted after `#pragma once`.

Rule:
- After `#pragma once`, the first non-comment, non-blank preprocessor directive
  must be `#ifndef ...`. If it is `#include ...` (or anything else), report an error.

Scope:
- Scans the local `src` tree (two levels up from this test directory).
- Only checks project headers ("*.h"). Third-party code is not scanned.

Exit codes:
- 0: No problems found
- 1: Found violations
"""

from __future__ import annotations

import os
import re
import sys
from typing import List, Optional, Tuple


THIS_DIR = os.path.abspath(os.path.dirname(__file__))
SRC_ROOT = os.path.abspath(os.path.join(THIS_DIR, "..", "..", "src"))


RE_PRAGMA_ONCE = re.compile(r"^\s*#\s*pragma\s+once\b")


def list_header_files(root: str) -> List[str]:
    out: List[str] = []
    for dirpath, _, filenames in os.walk(root):
        for fn in filenames:
            if not fn.endswith(".h"):
                continue
            # Skip hidden files
            if fn.startswith('.'):
                continue
            out.append(os.path.join(dirpath, fn))
    return out


def read_text(path: str) -> str:
    with open(path, "r", encoding="utf-8") as f:
        return f.read()


def find_pragma_once_line(lines: List[str]) -> Optional[int]:
    for i, ln in enumerate(lines):
        if RE_PRAGMA_ONCE.match(ln):
            return i
    return None


def classify_first_directive_after_pragma(lines: List[str], start_idx: int) -> Optional[Tuple[int, str]]:
    """Return (line_index, directive_keyword) of the first preprocessor directive after `#pragma once`.

    Skips blank lines and both line and block comments. Returns None if no directive found.
    `directive_keyword` is lower-cased one of: 'ifndef', 'include', or other (actual token).
    """
    in_block_comment = False
    i = start_idx + 1
    while i < len(lines):
        s = lines[i]
        p = 0
        while True:
            if in_block_comment:
                end = s.find('*/', p)
                if end == -1:
                    # Entire line is still in a block comment
                    i += 1
                    break
                in_block_comment = False
                p = end + 2
                # Continue scanning remainder of the same line
                continue

            # Skip leading whitespace
            while p < len(s) and s[p].isspace():
                p += 1

            # Empty after trimming -> move to next line
            if p >= len(s):
                i += 1
                break

            # Line comment
            if s.startswith('//', p):
                i += 1
                break

            # Block comment start
            if s.startswith('/*', p):
                in_block_comment = True
                p += 2
                continue

            # First non-comment token
            if s[p] == '#':
                rest = s[p+1:].lstrip()
                # Extract directive keyword
                m = re.match(r"(\w+)", rest)
                if not m:
                    return (i, "#")
                kw = m.group(1).lower()
                return (i, kw)

            # Non-preprocessor content before any directive: treat as benign; continue to next line
            i += 1
            break

    return None


def check_header(path: str) -> Optional[str]:
    text = read_text(path)
    lines = text.splitlines()

    idx = find_pragma_once_line(lines)
    if idx is None:
        return None  # No pragma once; out of scope

    res = classify_first_directive_after_pragma(lines, idx)
    if res is None:
        return None  # No directive found after pragma once; do not flag

    line_no, kw = res
    if kw == 'ifndef':
        return None
    if kw == 'include':
        return f"{path}:{line_no+1}: `#include` appears immediately after `#pragma once` (expected `#ifndef ...`)"
    return f"{path}:{line_no+1}: unexpected `#{kw}` after `#pragma once` (expected `#ifndef ...`)"


def main(argv: List[str]) -> int:
    root = SRC_ROOT
    if len(argv) > 1:
        root = os.path.abspath(argv[1])

    print(f"Scanning headers under: {root}")
    headers = list_header_files(root)

    problems: List[str] = []
    for h in headers:
        msg = check_header(h)
        if msg:
            problems.append(msg)

    if problems:
        print("Found violations:")
        for p in problems:
            print(p)
        return 1
    print("No violations found.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))


