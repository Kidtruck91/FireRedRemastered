"""Validate the current Orange region: map data, triggers, warps and seams.

The region is discovered from data/maps/map_groups.json rather than a name
prefix, so every map in a gMapGroup_Orange* group is covered however it is
spelled. Tilesets are resolved per layout from layouts.json.

Every check reports instead of raising, so one run lists all of the problems it
can find. Interpreting the coordinate scripts is a static read of the tracked
data, not an emulator: it tells you a landing cell is solid, not that a player
can reach the trigger.

Assertions that encode an older Navel/edge-barrier design live behind --legacy;
they are kept for reference but are not part of the current structural check.
Run --fixtures to confirm the checks still catch the faults they are meant to.
"""
import argparse
import ast
import json
import re
import struct
import sys
from collections import defaultdict
from pathlib import Path

from render_maps import ROOT, tileset_path

MB_NORMAL = 0x00
SURF = {0x10, 0x11, 0x12, 0x13, 0x15, 0x1A, 0x1B, 0x50, 0x51, 0x52, 0x53}
CURRENT = {0x50: (1, 0), 0x51: (-1, 0), 0x52: (0, -1), 0x53: (0, 1)}
DIRECTIONS = {'DIR_SOUTH': 1, 'DIR_NORTH': 2, 'DIR_WEST': 3, 'DIR_EAST': 4}
OPPOSITE = {'left': 'right', 'right': 'left', 'up': 'down', 'down': 'up'}
# A warp may legitimately arrive facing a door, stairwell or arrow-warp tile.
ARRIVAL_BEHAVIORS = {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B}


class Report:
    """Collects findings so a run reports everything it can see."""

    def __init__(self):
        self.items = []
        self.seen = set()
        self.tally = defaultdict(int)

    def fail(self, where, what, *detail):
        # The same fault is reached from several facings and from every cell
        # that feeds one current, so report each distinct finding once.
        key = (str(where), what, tuple(str(d) for d in detail))
        if key not in self.seen:
            self.seen.add(key)
            self.items.append((str(where), what, detail))
        return False

    def check(self, condition, where, what, *detail):
        return True if condition else self.fail(where, what, *detail)

    def count(self, key, n=1):
        self.tally[key] += n

    def dump(self, title):
        by_map = defaultdict(list)
        for where, what, detail in self.items:
            by_map[where].append((what, detail))
        print(f'== {title} ==')
        for key in sorted(self.tally):
            print(f'  {self.tally[key]:>6}  {key}')
        if not self.items:
            print('  no problems found')
            return 0
        print(f'\n  {len(self.items)} problem(s) in {len(by_map)} map(s):')
        for where in sorted(by_map):
            print(f'  {where}')
            for what, detail in by_map[where]:
                extra = ''.join(f'  {d}' for d in detail)
                print(f'      {what}{extra}')
        return 1


class World:
    def __init__(self):
        self.layouts = {l['id']: l for l in
                        json.loads((ROOT / 'data/layouts/layouts.json').read_text())['layouts']}
        self.maps = {}
        self.by_name = {}
        for path in (ROOT / 'data/maps').glob('*/map.json'):
            header = json.loads(path.read_text())
            self.maps[header['id']] = header
            self.by_name[header['name']] = header
        self.loaded = {}

    def load(self, header):
        key = header['layout']
        if key not in self.loaded:
            layout = self.layouts[key]
            blocks = [v[0] for v in
                      struct.iter_unpack('<H', (ROOT / layout['blockdata_filepath']).read_bytes())]
            attributes, metatiles = [], []
            for symbol in (layout['primary_tileset'], layout['secondary_tileset']):
                directory = tileset_path(symbol)
                attributes.extend(v[0] for v in struct.iter_unpack(
                    '<I', (directory / 'metatile_attributes.bin').read_bytes()))
                metatiles.extend(struct.iter_unpack(
                    '<8H', (directory / 'metatiles.bin').read_bytes()))
            self.loaded[key] = (layout, blocks, attributes, metatiles)
        return self.loaded[key]

    def cell(self, header, x, y):
        """(block, behavior) or None when the coordinate is off the map."""
        layout, blocks, attributes, _ = self.load(header)
        if not (0 <= x < layout['width'] and 0 <= y < layout['height']):
            return None
        block = blocks[y * layout['width'] + x]
        mid = block & 1023
        if mid >= len(attributes):
            return block, None
        return block, attributes[mid] & 511


def region(world):
    """Every map in a gMapGroup_Orange* group, in group order."""
    groups = json.loads((ROOT / 'data/maps/map_groups.json').read_text())
    names, group_of = [], {}
    for group in groups['group_order']:
        if 'Orange' not in group:
            continue
        for name in groups[group]:
            names.append(name)
            group_of[name] = group
    return [world.by_name[n] for n in names if n in world.by_name], group_of


class Script:
    """The subset of the script language these coordinate scripts use."""

    def __init__(self, header, report):
        self.name = header['name']
        self.report = report
        self.labels, self.code = {}, []
        path = ROOT / 'data/maps' / self.name / 'scripts.inc'
        self.overrides = {}
        if not path.exists():
            return
        text = path.read_text()
        for line_number, line in enumerate(text.splitlines(), 1):
            line = line.split('@', 1)[0].strip()
            if not line:
                continue
            label = re.fullmatch(r'(\w+)::?', line)
            if label:
                self.labels[label[1]] = len(self.code)
            else:
                command, *tail = line.split(None, 1)
                args = [v.strip() for v in tail[0].split(',')] if tail else []
                self.code.append((command, args, line_number))
        # Scripted floor anchors: a tile the map scripts rewrite at runtime.
        for x, y, _name, impassable in re.findall(
                r'setmetatile\s+(\d+)\s*,\s*(\d+)\s*,\s*(\w+)\s*,\s*(\d+)', text):
            self.overrides[int(x), int(y)] = int(impassable)

    # Commands with no bearing on where a coordinate script puts the player.
    IGNORED = {
        'lock', 'lockall', 'release', 'releaseall', 'faceplayer', 'end', 'return',
        'msgbox', 'message', 'waitmessage', 'closemessage', 'waitstate', 'delay',
        'playse', 'waitse', 'playfanfare', 'waitfanfare', 'playbgm', 'fadedefaultbgm',
        'setflag', 'clearflag', 'setvar_if', 'special', 'specialvar_', 'dofieldeffect',
        'applymovement', 'waitmovement', 'addobject', 'removeobject', 'showobjectat',
        'hideobjectat', 'setobjectxy', 'turnobject', 'copyobjectxytoperm',
        'setmetatile', 'DrawWholeMapView', 'fadescreen', 'fadescreenswapbuffers',
        'setweather', 'doweather', 'resetweather', 'bufferpartymonnick', 'buffermovename',
        'checkitemspace', 'giveitem_msg', 'additem', 'removeitem', 'checkitem',
        'trainerbattle_no_intro', 'trainerbattle_single', 'setrespawn', 'savebgm',
        'cleartrainerflag', 'settrainerflag', 'closemessage', 'hidemoneybox',
        'erasebox', 'drawmoneybox', 'updatemoneybox', 'random', 'callstd', 'checkflag',
        'textcolor', 'waitbuttonpress', 'braillemessage', 'setorcopyvar', 'copyvar',
    }
    # Conditionals this checker cannot resolve statically; both sides get walked.
    UNRESOLVED = re.compile(r'goto_if_(set|unset|defeated|undefeated|questlog)$')

    def trace(self, label, x, y, facing):
        """(outcomes, unsupported, errors) -- every warp the script can reach."""
        if label not in self.labels:
            return set(), set(), {'undefined trigger script ' + label}
        outcomes, unsupported, errors = set(), set(), set()
        start = (self.labels[label], (), None)
        stack, seen = [start], {start}

        def value(token, variables):
            if token in DIRECTIONS:
                return DIRECTIONS[token]
            if token.startswith('VAR_'):
                table = dict(variables)
                if token not in table:
                    raise KeyError(token)
                return table[token]
            return int(token, 0)

        def push(pc, variables, comparison):
            state = (pc, tuple(sorted(variables.items())), comparison)
            if state not in seen and len(seen) < 4000:
                seen.add(state)
                stack.append(state)

        while stack:
            pc, frozen, comparison = stack.pop()
            variables = dict(frozen)
            try:
                if not 0 <= pc < len(self.code):
                    errors.add('ran off the end of the file')
                    continue
                command, args, _line = self.code[pc]
                base = command.split('.')[0]
                if command == 'getplayerxy':
                    variables[args[0]], variables[args[1]] = x, y
                elif command == 'specialvar':
                    if args[1] != 'GetPlayerFacingDirection':
                        unsupported.add('specialvar ' + args[1])
                        continue
                    variables[args[0]] = facing
                elif command in ('setvar', 'addvar', 'subvar'):
                    operand = value(args[1], variables.items())
                    current = value(args[0], variables.items()) if command != 'setvar' else 0
                    variables[args[0]] = (operand if command == 'setvar' else
                                          current + operand if command == 'addvar' else
                                          current - operand) & 65535
                elif command == 'compare':
                    comparison = (value(args[0], variables.items())
                                  - value(args[1], variables.items()))
                elif command in ('warp', 'warpsilent'):
                    if len(args) == 3:
                        outcomes.add(('warp', args[0], value(args[1], variables.items()),
                                      value(args[2], variables.items())))
                    else:
                        unsupported.add('%s with %d args' % (command, len(args)))
                    continue
                elif command == 'goto':
                    if args[0] in self.labels:
                        push(self.labels[args[0]], variables, comparison)
                    else:
                        errors.add('missing branch ' + args[0])
                    continue
                elif command.startswith('goto_if_') or command in ('call', 'call_if_eq',
                                                                   'call_if_ne'):
                    target = args[-1]
                    table = {'goto_if_eq': lambda c: c == 0, 'goto_if_ne': lambda c: c != 0,
                             'goto_if_lt': lambda c: c < 0, 'goto_if_gt': lambda c: c > 0,
                             'goto_if_le': lambda c: c <= 0, 'goto_if_ge': lambda c: c >= 0}
                    if command in table and comparison is not None:
                        if table[command](comparison):
                            if target in self.labels:
                                push(self.labels[target], variables, comparison)
                            else:
                                errors.add('missing branch ' + target)
                        else:
                            push(pc + 1, variables, comparison)
                        continue
                    if command not in table and not self.UNRESOLVED.match(command) \
                            and not command.startswith('call'):
                        unsupported.add(command)
                        continue
                    # Unknown at check time: walk both sides.
                    if target in self.labels:
                        push(self.labels[target], variables, comparison)
                    push(pc + 1, variables, comparison)
                    continue
                elif base in self.IGNORED or command in self.IGNORED:
                    if command in ('end', 'return', 'release', 'releaseall'):
                        continue
                else:
                    unsupported.add(command)
                    continue
                push(pc + 1, variables, comparison)
            except KeyError as exc:
                errors.add('uninitialized variable %s' % exc.args[0])
            except (ValueError, IndexError) as exc:
                errors.add(str(exc))
        return outcomes, unsupported, errors


def structural(world, maps, report):
    """Map data: sizes, metatile ids, palettes, heights, events, currents."""
    triggers = {}
    for header in maps:
        name = header['name']
        layout, blocks, attributes, metatiles = world.load(header)
        w, h = layout['width'], layout['height']
        report.check(len(blocks) == w * h, name, 'block count does not match %dx%d' % (w, h))
        report.check((w + 15) * (h + 14) <= 10240, name,
                     'map too large for the backup buffer', '%dx%d' % (w, h))

        used = {v & 1023 for v in blocks}
        bad = sorted(m for m in used if m >= len(attributes))
        report.check(not bad, name, 'metatile id past the end of the tilesets',
                     ' '.join(hex(m) for m in bad[:6]))
        for mid in sorted(used - set(bad)):
            if any(v >> 12 > 12 for v in metatiles[mid]):
                report.fail(name, 'metatile uses an unloaded palette bank', hex(mid))
            if (attributes[mid] & 511) >= 0xF0 or (attributes[mid] >> 29) & 3 >= 3:
                report.fail(name, 'metatile has invalid attributes', hex(mid))

        for i, block in enumerate(blocks):
            mid = block & 1023
            if mid >= len(attributes) or block & 0xC00:
                continue
            # Underwater maps are the exception: the player is not surfing
            # there, they are walking the seafloor at elevation 3, and the
            # floor legitimately carries deep-water behaviour.
            if (attributes[mid] & 511 in SURF and block >> 12 not in (0, 1)
                    and header.get('map_type') != 'MAP_TYPE_UNDERWATER'):
                report.fail(name, 'Surf water at walking height',
                            '(%d,%d)' % (i % w, i // w), hex(block))
        report.count('map cells checked', len(blocks))

        for kind, limit in (('object_events', 255), ('warp_events', 255),
                            ('coord_events', 255), ('bg_events', 255)):
            report.check(len(header.get(kind) or []) <= limit, name,
                         '%s overflow' % kind, len(header.get(kind) or []))

        seen, by_position = set(), defaultdict(list)
        for event in header.get('coord_events') or []:
            x, y = event['x'], event['y']
            found = world.cell(header, x, y)
            if not report.check(found, name, 'trigger outside the map', '(%d,%d)' % (x, y)):
                continue
            block, _behavior = found
            report.check(not block & 0xC00, name, 'trigger on a solid cell', '(%d,%d)' % (x, y))
            report.check(event['elevation'] == 0 or block >> 12 in (0, 15, event['elevation']),
                         name, 'trigger elevation does not match the cell', '(%d,%d)' % (x, y))
            key = (x, y, event['elevation'], event.get('var'), event.get('var_value'))
            report.check(key not in seen, name, 'duplicate trigger condition', key)
            seen.add(key)
            by_position[x, y].append(event)
            report.count('coordinate events')
        triggers[header['id']] = by_position

        for event in header.get('warp_events') or []:
            report.check(world.cell(header, event['x'], event['y']), name,
                         'warp outside the map', '(%d,%d)' % (event['x'], event['y']))
            destination = world.maps.get(event['dest_map'])
            if not report.check(destination, name, 'warp to an unknown map', event['dest_map']):
                continue
            index = int(event['dest_warp_id'])
            report.check(0 <= index < len(destination.get('warp_events') or []), name,
                         'warp id %d missing in %s' % (index, destination['name']))
            report.count('warps')

        for i, block in enumerate(blocks):
            mid = block & 1023
            if block & 0xC00 or mid >= len(attributes) or attributes[mid] & 511 not in CURRENT:
                continue
            report.count('forced-current cells')
            visited, cell, broke = set(), i, False
            while not broke:
                mid = blocks[cell] & 1023
                if mid >= len(attributes) or attributes[mid] & 511 not in CURRENT:
                    break
                if cell in visited:
                    report.fail(name, 'current loops forever',
                                'from (%d,%d)' % (i % w, i // w))
                    break
                visited.add(cell)
                dx, dy = CURRENT[attributes[mid] & 511]
                x, y = cell % w + dx, cell // w + dy
                found = world.cell(header, x, y)
                if not found:
                    report.fail(name, 'current pushes off the map', '(%d,%d)' % (x, y))
                    break
                target, behavior = found
                if target & 0xC00 or target >> 12 != 1 or behavior not in SURF:
                    # Being pushed into a wall is how a current ends, and being
                    # pushed onto plain walkable ground at the same height is a
                    # dismount -- that is how the Mikan Gym sets the player down
                    # on each of its rocks. Anything else is a real problem.
                    dismount = (not target & 0xC00 and target >> 12 == 1
                                and behavior == MB_NORMAL)
                    if not target & 0xC00 and not dismount:
                        report.fail(name, 'current ends on an unsafe cell',
                                    '(%d,%d)' % (x, y), hex(target))
                    break
                cell = y * w + x
    return triggers


def seams(world, maps, report):
    """Every connection must be mirrored by the map on the other side."""
    names = {h['id'] for h in maps}
    for header in maps:
        for link in header.get('connections') or []:
            other = world.maps.get(link['map'])
            if not report.check(other, header['name'], 'connection to an unknown map', link['map']):
                continue
            if link['direction'] not in OPPOSITE:
                continue
            back = [c for c in (other.get('connections') or [])
                    if c['map'] == header['id'] and c['direction'] == OPPOSITE[link['direction']]]
            if not report.check(back, header['name'], 'connection not mirrored by %s' % other['name'],
                                link['direction']):
                continue
            report.check(back[0]['offset'] == -link['offset'], header['name'],
                         'connection offset does not match %s' % other['name'],
                         '%d vs %d' % (link['offset'], back[0]['offset']))
            report.count('connection seams')
            if other['id'] not in names:
                report.count('seams leaving the region')


def landings(world, maps, triggers, report):
    """Walk every coordinate script for all four facings and check where it puts you."""
    programs = {h['id']: Script(h, report) for h in maps}
    unsupported = defaultdict(set)
    for header in maps:
        program = programs[header['id']]
        for event in header.get('coord_events') or []:
            for facing in DIRECTIONS.values():
                outcomes, missing, errors = program.trace(
                    event['script'], event['x'], event['y'], facing)
                unsupported[header['name']] |= missing
                for why in errors:
                    report.fail(header['name'], 'script ' + why, event['script'])
                for _, dest, x, y in outcomes:
                    destination = world.maps.get(dest)
                    if not report.check(destination, header['name'],
                                        'script warps to an unknown map', dest):
                        continue
                    found = world.cell(destination, x, y)
                    if not report.check(found, header['name'], 'script warp lands off the map',
                                        '%s (%d,%d)' % (destination['name'], x, y)):
                        continue
                    block, behavior = found
                    report.count('script warp paths')
                    where = '%s (%d,%d)' % (destination['name'], x, y)
                    override = (programs[dest].overrides.get((x, y))
                                if dest in programs else None)
                    if override is not None:
                        report.count('landings on scripted floor anchors')
                        if override:
                            report.fail(header['name'],
                                        'script warp lands on a cell its map blocks', where)
                        continue
                    if block & 0xC00 and behavior not in ARRIVAL_BEHAVIORS:
                        report.fail(header['name'], 'script warp lands on a solid cell',
                                    where, hex(block))
                    if triggers.get(dest, {}).get((x, y)):
                        report.fail(header['name'], 'script warp lands on another trigger', where)
                    if re.search(r'SeaEdge|SeaCorner|SailTo', event['script']):
                        # A designed shore landing may dismount onto walking ground.
                        # Water landings keep water height; dry ground must be walkable.
                        heights = (0, 1) if behavior in SURF else (0, 3)
                        if block >> 12 not in heights:
                            report.fail(header['name'],
                                        'sea crossing arrives at the wrong height',
                                        where, hex(block))
    for name in sorted(unsupported):
        if unsupported[name]:
            report.fail(name, 'script commands this checker does not interpret',
                        ' '.join(sorted(unsupported[name])))
    return programs


def legacy(world, triggers, programs, report):
    """Checks tied to an older Navel/edge-barrier design. Kept for reference."""
    table = ROOT / '_follower_port_work/patches/edge_links.py'
    if table.exists():
        tree = ast.parse(table.read_text())
        edges = next((ast.literal_eval(n.value) for n in tree.body if isinstance(n, ast.Assign)
                      and any(isinstance(t, ast.Name) and t.id == 'EDGES' for t in n.targets)), [])
        side_facing = {'bottom': 1, 'top': 2, 'left': 3, 'right': 4}
        for name, side, dest, _offset, _clamp, _across in edges:
            header = world.maps.get('MAP_ORANGE_ISLANDS_' + name.upper())
            if header is None:
                report.fail('edge_links.py', 'links a map that no longer exists', name)
                continue
            layout, _, _, _ = world.load(header)
            w, h = layout['width'], layout['height']
            cells = {'top': [(x, 0) for x in range(w)], 'bottom': [(x, h - 1) for x in range(w)],
                     'left': [(0, y) for y in range(h)],
                     'right': [(w - 1, y) for y in range(h)]}[side]
            for x, y in cells:
                found = world.cell(header, x, y)
                if not found:
                    continue
                block, behavior = found
                if block & 0xC00 or behavior not in SURF or block >> 12 != 1:
                    continue
                matches = triggers[header['id']][x, y]
                if not report.check(len(matches) == 1, header['name'],
                                    'open sea edge without exactly one trigger',
                                    '(%d,%d)' % (x, y), len(matches)):
                    continue
                outcomes, _, _ = programs[header['id']].trace(
                    matches[0]['script'], x, y, side_facing[side])
                report.check(any(o[1] == 'MAP_ORANGE_ISLANDS_' + dest.upper() for o in outcomes),
                             header['name'], 'edge trigger does not cross to %s' % dest,
                             '(%d,%d)' % (x, y), sorted(o[1] for o in outcomes))
                report.count('legacy sea-edge crossings')

    navel = world.maps.get('MAP_ORANGE_ISLANDS_NAVEL')
    if navel:
        for x0, y0 in [(74, 42), (67, 48), (67, 52)]:
            for y in range(y0, y0 + 2):
                for x in range(x0, x0 + 6):
                    found = world.cell(navel, x, y)
                    if not found:
                        continue
                    block, behavior = found
                    report.check(not block & 0xC00 and block >> 12 == 3 and behavior not in SURF,
                                 'Navel', 'pier deck is not walkable', '(%d,%d)' % (x, y))
        for points, label in [([(24, 1), (58, 1)], 'Navel')]:
            for x, y in points:
                found = world.cell(navel, x, y)
                if found:
                    report.check(found[0] & 0xC00, label,
                                 'cell the old barrier design expected to be solid',
                                 '(%d,%d)' % (x, y), hex(found[0]))


FIXTURES = """\
blocked landing      a script warp onto a solid cell
wrong water height   Surf water stored at walking elevation
duplicate trigger    two coord events with the same condition
missing warp target  a warp pointing past the destination's warp list
"""


def fixtures(report):
    """Feed each check a deliberately broken map and confirm it objects."""
    import copy
    world = World()
    maps, _ = region(world)
    base = next(h for h in maps if (h.get('coord_events') or []))

    results = []

    # blocked landing / missing warp target / duplicate trigger operate on headers
    probe = Report()
    broken = copy.deepcopy(base)
    broken['warp_events'] = [{'x': 0, 'y': 0, 'elevation': 0,
                              'dest_map': base['id'], 'dest_warp_id': '999'}]
    structural(world, [broken], probe)
    results.append(('missing warp target',
                    any('warp id' in w for _, w, _ in probe.items)))

    probe = Report()
    broken = copy.deepcopy(base)
    event = dict(broken['coord_events'][0])
    broken['coord_events'] = [event, dict(event)]
    structural(world, [broken], probe)
    results.append(('duplicate trigger',
                    any('duplicate trigger' in w for _, w, _ in probe.items)))

    # wrong water height needs block data, so patch a loaded layout in memory
    probe = Report()
    world2 = World()
    maps2, _ = region(world2)
    target = next((h for h in maps2
                   if any((world2.load(h)[2][b & 1023] & 511) in SURF and not b & 0xC00
                          for b in world2.load(h)[1])), None)
    if target:
        layout, blocks, attributes, metatiles = world2.load(target)
        blocks = list(blocks)
        for i, b in enumerate(blocks):
            if not b & 0xC00 and (attributes[b & 1023] & 511) in SURF:
                blocks[i] = (b & 0x0FFF) | (3 << 12)
                break
        world2.loaded[target['layout']] = (layout, blocks, attributes, metatiles)
        structural(world2, [target], probe)
    results.append(('wrong water height',
                    any('Surf water at walking height' in w for _, w, _ in probe.items)))

    probe = Report()
    world3 = World()
    maps3, _ = region(world3)
    victim = next((h for h in maps3 if (h.get('coord_events') or [])
                   and (ROOT / 'data/maps' / h['name'] / 'scripts.inc').exists()), None)
    caught = False
    if victim:
        tri = structural(world3, maps3, Report())
        programs = {h['id']: Script(h, probe) for h in maps3}
        for header in maps3:
            for event in header.get('coord_events') or []:
                outs, _, _ = programs[header['id']].trace(
                    event['script'], event['x'], event['y'], 1)
                for res in sorted(outs):
                    dest = world3.maps.get(res[1])
                    if dest is None:
                        continue
                    lay, blocks, attrs, metas = world3.load(dest)
                    blocks = list(blocks)
                    idx = res[3] * lay['width'] + res[2]
                    if 0 <= idx < len(blocks):
                        blocks[idx] = (blocks[idx] & ~0xC00) | (1 << 10)
                        world3.loaded[dest['layout']] = (lay, blocks, attrs, metas)
                        landings(world3, [header], tri, probe)
                        caught = any('solid cell' in w for _, w, _ in probe.items)
                    break
            if caught:
                break
    results.append(('blocked landing', caught))

    print('== regression fixtures ==')
    bad = 0
    for label, passed in results:
        print('  %-22s %s' % (label, 'detected' if passed else 'NOT DETECTED'))
        bad += 0 if passed else 1
    return 1 if bad else 0


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--rom', action='store_true',
                        help='also confirm each map binary is embedded in pokefirered.gba')
    parser.add_argument('--legacy', action='store_true',
                        help='also run the checks tied to the older barrier design')
    parser.add_argument('--fixtures', action='store_true',
                        help='self-test: confirm the checks still catch known faults')
    args = parser.parse_args()

    if args.fixtures:
        sys.exit(fixtures(Report()))

    world = World()
    maps, group_of = region(world)
    report = Report()
    report.count('maps in gMapGroup_Orange* groups', len(maps))

    # The group a map sits in is how the region is organised; a cave filed under
    # the wrong island still builds, so nothing else would notice.
    for header in maps:
        name, group = header['name'], group_of[header['name']]
        island = re.fullmatch(r'gMapGroup_Orange_(\w+)_Interior', group)
        if island and not re.match(r'OrangeIslands?_' + island[1] + r'(_|$)', name):
            report.fail(name, 'filed under %s' % group)

    triggers = structural(world, maps, report)
    seams(world, maps, report)
    programs = landings(world, maps, triggers, report)

    if args.rom:
        rom_path = ROOT / 'pokefirered.gba'
        if not rom_path.exists():
            report.fail('pokefirered.gba', 'no ROM to compare against; build first')
        else:
            rom = rom_path.read_bytes()
            for header in maps:
                layout, _, _, _ = world.load(header)
                data = (ROOT / layout['blockdata_filepath']).read_bytes()
                report.check(data in rom, header['name'],
                             'map data in the ROM is stale; rebuild')
                report.count('map binaries found in the ROM')

    status = report.dump('current Orange region')

    if args.legacy:
        old = Report()
        legacy(world, triggers, programs, old)
        print()
        old.dump('historical design checks (not part of the current region)')

    sys.exit(status)


if __name__ == '__main__':
    main()
