#!/bin/sh
# Regression check: an empty .include must still produce a valid line marker.
#
# AsmFile used to return from its constructor on a zero-byte file before setting
# m_pos/m_lineNum/m_lineStart/m_buffer, so OutputLocation() emitted a marker built
# from uninitialized memory -- "invalid preprocessing directive #-" at whatever
# include happened to be empty. Because it read uninitialized stack, it failed on
# some machines and not others, so this check has to assert the exact output.
#
# Usage: tools/preproc/test_empty_include.sh [path/to/preproc] [path/to/charmap.txt]
set -e

here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
root=$(CDPATH= cd -- "$here/../.." && pwd)
preproc=${1:-$here/preproc}
charmap=${2:-$root/charmap.txt}

if [ ! -x "$preproc" ]; then
    echo "test_empty_include: no preproc at $preproc; run make -C tools/preproc" >&2
    exit 2
fi

# The checks run from a scratch directory, so resolve both inputs first.
preproc=$(CDPATH= cd -- "$(dirname -- "$preproc")" && pwd)/$(basename -- "$preproc")
charmap=$(CDPATH= cd -- "$(dirname -- "$charmap")" && pwd)/$(basename -- "$charmap")

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"

: > empty.inc
printf '\t.include "empty.inc"\n\t.byte 1\n' > top.s

actual=$("$preproc" top.s "$charmap")
expected='# 1 "empty.inc"
# 2 "top.s"
	.byte 1'

if [ "$actual" != "$expected" ]; then
    echo 'test_empty_include: FAILED' >&2
    echo '--- expected ---' >&2
    printf '%s\n' "$expected" >&2
    echo '--- actual ---' >&2
    printf '%s\n' "$actual" >&2
    exit 1
fi

# Repeat runs must agree; the old fault varied with whatever was on the stack.
for _ in 1 2 3 4 5; do
    if [ "$("$preproc" top.s "$charmap")" != "$expected" ]; then
        echo 'test_empty_include: FAILED (output is not stable across runs)' >&2
        exit 1
    fi
done

echo 'test_empty_include: ok'
