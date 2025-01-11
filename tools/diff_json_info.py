from pathlib import Path
from os import path
import json
from pprint import pp

INFO_PATH = "../res/mc/info"

Path("info_diff/tag").mkdir(parents=True, exist_ok=True)

def retrieveInfo(filename: str, version: str):
    filepath = path.join(INFO_PATH, version, filename + '.json')
    info = None
    with open(filepath) as f:
        info = json.load(f)
    
    ret = {}
    base = None
    if 'base' in info:
        ret, prev_base = retrieveInfo(filename, info['base'])
        if len(info) == 1:
            if prev_base is not None:
                base = prev_base
            else:
                base = info['base']
        del info['base']        
        
    if 'removed' in info:
        for key in info['removed']:
            if key in ret:
                del ret[key]
        del info['removed']
    
    if 'added' in info:
        ret.update(info['added'])
    else:
        ret.update(info)
        
    return ret, base

def dictDiff(first: dict, second: dict):
    ret = {}
    removed = []
    for key, value in first.items():
        if not key in second:
            # print('Key removed:', key)
            removed.append(key)
    if removed:
        ret['removed'] = removed
    
    added = {}
    for key, value in second.items():
        if not key in first:
            # print('Key added:', key)
            added[key] = value
    if added:
        ret['added'] = added
    
    return ret

def getInfoDiff(filename: str, firstVer: str):
    print(filename, firstVer)
    second = None
    with open(path.join('data_collectors', filename + '.json')) as f:
        second = json.load(f)
    first, base = retrieveInfo(filename, firstVer)
    print(base)
    ret = dictDiff(first, second['added'])
    ret['base'] = firstVer
    with open(path.join('info_diff', filename + '.json'), 'w+') as f:
        f.write(json.dumps(ret, sort_keys=True))
    return ret

pp(getInfoDiff('attribute', '1.21'))
# pp(getInfoDiff('biome', '1.20'))
# pp(getInfoDiff('block', '1.20'))
#pp(getInfoDiff('dimension', '1.17'))
# pp(getInfoDiff('effect', '1.19'))
# pp(getInfoDiff('enchantment', '1.20.6'))
# pp(getInfoDiff('entity', '1.20.6'))
#pp(getInfoDiff('feature', '1.17'))
#pp(getInfoDiff('fluid', '1.17'))
pp(getInfoDiff('item', '1.21'))
pp(getInfoDiff('tag/block', '1.21'))
pp(getInfoDiff('tag/entity_type', '1.21'))
pp(getInfoDiff('tag/fluid', '1.21'))
pp(getInfoDiff('tag/instrument', '1.21'))
pp(getInfoDiff('tag/item', '1.21'))
pp(getInfoDiff('tag/painting_variant', '1.21'))
pp(getInfoDiff('tag/point_of_interest_type', '1.21'))
pp(getInfoDiff('tag/biome', '1.21'))
pp(getInfoDiff('tag/game_event', '1.21'))
pp(getInfoDiff('tag/structure', '1.21'))
pp(getInfoDiff('tag/world_preset', '1.21'))
pp(getInfoDiff('tag/banner_pattern', '1.21'))
pp(getInfoDiff('tag/cat_variant', '1.21'))
pp(getInfoDiff('tag/flat_level_generator_preset', '1.21'))
pp(getInfoDiff('tag/damage_type', '1.21'))
pp(getInfoDiff('tag/enchantment', '1.21'))
# pp(getInfoDiff('tag/configured_structure_feature', '1.19'))
