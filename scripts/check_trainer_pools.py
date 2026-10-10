#!/usr/bin/env python3
"""Checks the _Advanced trainer pools in src/data/trainer_pools.h.

Every entry in a pool whose name ends in _Advanced must:
  - be fully evolved (no evolution in src/data/pokemon/evolution.h)
  - have four moves
  - hold an item
  - choose an ability (.ability = 1, 2 or 3)
Run automatically by `make`; exits non-zero (stopping the build) if something is wrong.
"""
import re
import sys

POOLS = 'src/data/trainer_pools.h'
EVOS = 'src/data/pokemon/evolution.h'


def strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    return re.sub(r'//[^\n]*', '', text)


def species_with_evolutions():
    text = strip_comments(open(EVOS).read())
    return set(re.findall(r'\[(SPECIES_\w+)\]\s*=\s*\{\s*\{', text))


def top_level_entries(body):
    """Split '{...}, {...}, POOL_END' into the top-level brace groups."""
    entries, depth, start = [], 0, None
    for i, ch in enumerate(body):
        if ch == '{':
            if depth == 0:
                start = i
            depth += 1
        elif ch == '}':
            depth -= 1
            if depth == 0 and start is not None:
                entries.append(body[start + 1:i])
                start = None
    return entries


def main():
    evolves = species_with_evolutions()
    text = strip_comments(open(POOLS).read())
    errors = []
    for m in re.finditer(r'struct\s+PoolMon\s+(\w+_Advanced)\s*\[\s*\]\s*=\s*\{', text):
        name = m.group(1)
        # find the matching closing brace of the array
        depth, i = 1, m.end()
        while depth and i < len(text):
            depth += {'{': 1, '}': -1}.get(text[i], 0)
            i += 1
        for entry in top_level_entries(text[m.end():i - 1]):
            sp = re.search(r'\.species\s*=\s*(SPECIES_\w+)', entry)
            if not sp or sp.group(1) == 'SPECIES_NONE':
                continue
            species = sp.group(1)
            where = '%s: %s' % (name, species)
            if species in evolves:
                errors.append('%s is not fully evolved' % where)
            mv = re.search(r'\.moves\s*=\s*\{([^}]*)\}', entry)
            moves = [x.strip() for x in mv.group(1).split(',') if x.strip()] if mv else []
            if len([x for x in moves if x not in ('MOVE_NONE', '0')]) != 4:
                errors.append('%s needs four moves' % where)
            item = re.search(r'\.heldItem\s*=\s*(\w+)', entry)
            if not item or item.group(1) in ('ITEM_NONE', '0'):
                errors.append('%s needs a held item' % where)
            ab = re.search(r'\.ability\s*=\s*(\d+)', entry)
            if not ab or ab.group(1) not in ('1', '2', '3'):
                errors.append('%s needs .ability = 1, 2 or 3' % where)
    if errors:
        print('Trainer pool check failed (src/data/trainer_pools.h):', file=sys.stderr)
        for e in errors:
            print('  ' + e, file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
