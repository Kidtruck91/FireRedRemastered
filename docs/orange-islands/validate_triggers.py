"""Validate current Orange trigger scripts, movement cells and Surf arrivals.

Interprets the small set of event commands used by these coordinate scripts,
including direction-sensitive corners and both sides of their clamp branches.
This checks the actual tracked scripts/data; it is not a game emulator.
"""
import argparse
import ast
from collections import defaultdict
import json
from pathlib import Path
import re
import struct

from render_maps import ROOT, tileset_path

SURF = {0x10,0x11,0x12,0x13,0x15,0x1A,0x1B,0x50,0x51,0x52,0x53}
CURRENT = {0x50:(1,0),0x51:(-1,0),0x52:(0,-1),0x53:(0,1)}
DIRECTIONS = {'DIR_SOUTH':1, 'DIR_NORTH':2, 'DIR_WEST':3, 'DIR_EAST':4}
SIDE_DIRECTION = {'bottom':1, 'top':2, 'left':3, 'right':4}


class World:
    def __init__(self):
        self.layouts = {l['id']:l for l in json.loads((ROOT/'data/layouts/layouts.json').read_text())['layouts']}
        self.maps = {j['id']:j for p in (ROOT/'data/maps').glob('*/map.json') if (j:=json.loads(p.read_text()))}
        self.loaded = {}

    def load(self, header):
        key = header['layout']
        if key not in self.loaded:
            layout = self.layouts[key]
            raw = (ROOT/layout['blockdata_filepath']).read_bytes()
            blocks = [v[0] for v in struct.iter_unpack('<H',raw)]
            attributes = []; metatiles = []
            for symbol in (layout['primary_tileset'],layout['secondary_tileset']):
                directory = tileset_path(symbol)
                attributes.extend(v[0] for v in struct.iter_unpack('<I',(directory/'metatile_attributes.bin').read_bytes()))
                metatiles.extend(struct.iter_unpack('<8H',(directory/'metatiles.bin').read_bytes()))
            self.loaded[key] = (layout,blocks,attributes,metatiles)
        return self.loaded[key]

    def cell(self, header, x, y):
        layout,blocks,attributes,_ = self.load(header)
        assert 0<=x<layout['width'] and 0<=y<layout['height'], (header['name'],'outside map',x,y)
        block = blocks[y*layout['width']+x]; mid = block&1023
        assert mid<len(attributes), (header['name'],'undefined metatile',hex(mid))
        return block, attributes[mid]&511


class Script:
    def __init__(self, header):
        self.name = header['name']; self.labels = {}; self.code = []
        path = ROOT/'data/maps'/self.name/'scripts.inc'
        for line_number,line in enumerate(path.read_text().splitlines(),1):
            line = line.split('@',1)[0].strip()
            if not line: continue
            label = re.fullmatch(r'(\w+)::?',line)
            if label:
                self.labels[label[1]] = len(self.code)
            else:
                command,*tail = line.split(None,1)
                args = [v.strip() for v in tail[0].split(',')] if tail else []
                self.code.append((command,args,line_number))

    def trace(self, label, x, y, facing):
        assert label in self.labels, (self.name,'undefined trigger script',label)
        pc = self.labels[label]; variables = {}; comparison = None
        def value(token):
            if token in DIRECTIONS: return DIRECTIONS[token]
            if token.startswith('VAR_'):
                assert token in variables, (self.name,'uninitialized variable',token)
                return variables[token]
            return int(token,0)
        for _ in range(150):
            command,args,line = self.code[pc]; pc += 1
            context = (self.name,line,label,x,y,facing)
            if command=='getplayerxy':
                variables[args[0]],variables[args[1]] = x,y
            elif command=='specialvar':
                assert args[1]=='GetPlayerFacingDirection', context
                variables[args[0]] = facing
            elif command in ('setvar','addvar','subvar'):
                operand = value(args[1])
                variables[args[0]] = (operand if command=='setvar' else
                    value(args[0])+operand if command=='addvar' else value(args[0])-operand)&65535
            elif command=='compare':
                comparison = value(args[0])-value(args[1])
            elif command=='goto' or command.startswith('goto_if_'):
                take = command=='goto'
                if not take:
                    assert comparison is not None, context
                    take = {'goto_if_eq':comparison==0,'goto_if_ne':comparison!=0,
                            'goto_if_lt':comparison<0,'goto_if_gt':comparison>0,
                            'goto_if_le':comparison<=0,'goto_if_ge':comparison>=0}[command]
                if take:
                    assert args[0] in self.labels, (context,'missing branch',args[0])
                    pc = self.labels[args[0]]
            elif command=='warp':
                assert len(args)==3, context
                return args[0],value(args[1]),value(args[2])
            elif command=='end':
                return None
            else:
                raise AssertionError((context,'unsupported script command',command))
        raise AssertionError((self.name,label,'script does not terminate'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rom',action='store_true')
    args = parser.parse_args()
    world = World(); rom = (ROOT/'pokefirered.gba').read_bytes() if args.rom else None
    orange = [j for j in world.maps.values() if j['name'].startswith('OrangeIslands_')]
    programs = {}; positions = {}; cases = 0; events = 0; currents = 0; corners = set()
    for header in orange:
        name = header['name']; layout,blocks,attributes,metatiles = world.load(header)
        w,h = layout['width'],layout['height']
        assert len(blocks)==w*h and (w+15)*(h+14)<=10240, (name,'map data/buffer size')
        used = {v&1023 for v in blocks}
        assert all(mid<len(attributes) for mid in used), (name,'missing metatile')
        assert all(v>>12<=12 for mid in used for v in metatiles[mid]), (name,'unloaded palette')
        assert all((attributes[mid]&511)<0xF0 and (attributes[mid]>>29)&3<3 for mid in used), (name,'invalid attributes')
        for i,block in enumerate(blocks):
            if not block&0xC00 and attributes[block&1023]&511 in SURF:
                assert block>>12 in (0,1), (name,'Surf water at walking height',i%w,i//w,hex(block))
        assert len(header['coord_events'])<=255, (name,'coordinate event overflow')
        by_position = defaultdict(list); conditions = set()
        for event in header['coord_events']:
            x,y = event['x'],event['y']; block,_ = world.cell(header,x,y)
            assert not block&0xC00, (name,'blocked trigger',x,y)
            assert event['elevation']==0 or block>>12 in (0,15,event['elevation']), (name,'trigger elevation',x,y)
            condition = (x,y,event['elevation'],event.get('var'),event.get('var_value'))
            assert condition not in conditions, (name,'overlapping trigger',condition)
            conditions.add(condition); by_position[x,y].append(event)
        positions[header['id']] = by_position
        program = programs[header['id']] = Script(header)
        for event in header['warp_events']:
            world.cell(header,event['x'],event['y'])
            destination = world.maps[event['dest_map']]
            index = int(event['dest_warp_id'])
            assert 0<=index<len(destination['warp_events']), (name,'missing warp destination')
        for i,block in enumerate(blocks):
            if block&0xC00 or attributes[block&1023]&511 not in CURRENT: continue
            currents += 1; visited = set(); cell = i
            while attributes[blocks[cell]&1023]&511 in CURRENT:
                assert cell not in visited, (name,'current loop',i%w,i//w)
                visited.add(cell); dx,dy = CURRENT[attributes[blocks[cell]&1023]&511]
                x,y = cell%w+dx,cell//w+dy
                target,behavior = world.cell(header,x,y)
                assert not target&0xC00 and target>>12==1 and behavior in SURF, (name,'bad current step',x,y)
                cell = y*w+x
        if rom is not None:
            assert (ROOT/layout['blockdata_filepath']).read_bytes() in rom, (name,'stale ROM map')

    # Evaluate the actual branch instructions for every event and facing, not
    # just a representative entry or a regular-expression guess at a clamp.
    for header in orange:
        program = programs[header['id']]
        for event in header['coord_events']:
            events += 1
            for facing in DIRECTIONS.values():
                result = program.trace(event['script'],event['x'],event['y'],facing)
                if result is None: continue
                cases += 1; dest,x,y = result; destination = world.maps[dest]
                block,behavior = world.cell(destination,x,y)
                assert not block&0xC00, (header['name'],'warp lands on solid tile',result,hex(block))
                assert not positions[dest].get((x,y)), (header['name'],'warp lands on another trigger',result)
                if 'SeaEdge' in event['script'] or 'SeaCorner' in event['script'] or 'SailTo' in event['script']:
                    # A designed shore landing may dismount onto walking
                    # ground (e.g. Trovita's river bank). Water landings must
                    # retain water height; dry ground must be walkable height.
                    heights = (0,1) if behavior in SURF else (0,3)
                    assert block>>12 in heights, (header['name'],'wrong sea/shore arrival height',result)

    # Independent coverage/routing checks against the authored link table.
    tree = ast.parse((ROOT/'_follower_port_work/patches/edge_links.py').read_text())
    edges = next(ast.literal_eval(n.value) for n in tree.body if isinstance(n,ast.Assign)
                 and any(isinstance(t,ast.Name) and t.id=='EDGES' for t in n.targets))
    edge_checks = 0
    for name,side,dest,offset,clamp,across in edges:
        header = world.maps['MAP_ORANGE_ISLANDS_'+name.upper()]
        layout,_,_,_ = world.load(header); w,h = layout['width'],layout['height']
        cells = {'top':[(x,0) for x in range(w)],'bottom':[(x,h-1) for x in range(w)],
                 'left':[(0,y) for y in range(h)],'right':[(w-1,y) for y in range(h)]}[side]
        for x,y in cells:
            block,behavior = world.cell(header,x,y)
            if block&0xC00 or behavior not in SURF or block>>12!=1: continue
            matches = positions[header['id']][x,y]
            assert len(matches)==1, (name,'edge missing a unique trigger',x,y)
            event = matches[0]; facing = SIDE_DIRECTION[side]
            result = programs[header['id']].trace(event['script'],x,y,facing)
            assert result and result[0]=='MAP_ORANGE_ISLANDS_'+dest.upper(), (name,'wrong direction at edge/corner',x,y,side,result)
            edge_checks += 1
            if 'SeaCorner' in event['script']: corners.add((name,x,y))

    # The deck fronts have to be reachable from their actual shore cells on foot.
    navel = world.maps['MAP_ORANGE_ISLANDS_NAVEL']; deck_tiles = 0
    for x0,y0 in [(74,42),(67,48),(67,52)]:
        shore,behavior = world.cell(navel,x0-1,y0)
        assert not shore&0xC00 and shore>>12==3 and behavior not in SURF
        for y in range(y0,y0+2):
            for x in range(x0,x0+6):
                block,behavior = world.cell(navel,x,y)
                assert not block&0xC00 and block>>12==3 and behavior not in SURF, ('Navel','unwalkable pier',x,y)
                deck_tiles += 1
        for x in range(x0,x0+6):
            block,behavior = world.cell(navel,x,y0+2)
            assert block>>12==1 and behavior in SURF, ('Navel','pier water trim changed',x,y0+2)
    assert not navel['warp_events'] and not world.maps['MAP_ORANGE_ISLANDS_NAVEL_BEACH']['warp_events'], 'Stale Navel warps'
    valencia = world.maps['MAP_ORANGE_ISLANDS_VALENCIA_BEACH']
    for x,y in [(17,4),*[(x,36) for x in range(64,73)]]:
        block,behavior = world.cell(valencia,x,y)
        assert not block&0xC00 and block>>12==3 and behavior not in SURF, ('Valencia Beach','wrong shore height',x,y)
    for header,points in [(navel,[(24,1),(58,1)]),
                          (world.maps['MAP_ORANGE_ISLANDS_KINNOW'],[(x,9) for x in range(38,43)])]:
        for x,y in points:
            assert world.cell(header,x,y)[0]&0xC00, (header['name'],'passable rock/roof',x,y)

    route = world.maps['MAP_ORANGE_ISLANDS_ROUTE_3']; beach = world.maps['MAP_ORANGE_ISLANDS_MIKAN_BEACH']
    forward = next(c for c in route['connections'] if c['map']==beach['id'])
    reverse = next(c for c in beach['connections'] if c['map']==route['id'])
    assert forward['direction']=='right' and reverse['direction']=='left' and forward['offset']==-reverse['offset'], 'Mikan connection offsets'
    route_layout,_,_,_ = world.load(route); beach_layout,_,_,_ = world.load(beach)
    for y in range(route_layout['height']):
        by = y-forward['offset']
        if not 0<=by<beach_layout['height']: continue
        a,ab = world.cell(route,route_layout['width']-1,y); b,bb = world.cell(beach,0,by)
        if a&0xC00 or b&0xC00: continue
        # A natural shore may meet sea across a map seam. It needs a correct
        # east-facing shore edge/corner, not an uncapped interior sand strip.
        assert (ab in SURF)==(bb in SURF) or a&1023 in {0x10E,0x116,0x11E}, ('Mikan','uncapped coast',y,by)

    print(f'{len(orange)} maps; {events} coordinate events / {cases} script paths: valid landings, labels, heights and no overlapping triggers.')
    print(f'{edge_checks} sea-edge crossings, including both directions at {len(corners)} corners, passed.')
    print(f'{deck_tiles} pier deck cells and their water trim; reciprocal Mikan offsets; {currents} forced-current cells passed.')
    print('Water heights in all 45 maps, metatile IDs, behavior/layer values and loaded palette banks passed.')
    if rom is not None: print('All 45 current Orange map binaries are embedded in the built ROM.')


if __name__=='__main__':
    main()
