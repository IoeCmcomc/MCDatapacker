from pathlib import Path
from os import path
import json
from pprint import pp

INFO_PATH = "../resource/minecraft/info"

Path("info_diff/tag").mkdir(parents=True, exist_ok=True)

def retrieveInfo(filename: str, version: str):
    filepath = path.join(INFO_PATH, version, filename + '.json')
    info = None
    with open(filepath) as f:
        info = json.load(f)
    
    ret = {}
    if 'base' in info:
        ret = retrieveInfo(filename, info['base'])
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
        
    return ret    

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
    second = None
    with open(path.join('data_collectors', filename + '.json')) as f:
        second = json.load(f)
    ret = dictDiff(retrieveInfo(filename, firstVer), second['added'])
    ret['base'] = firstVer
    with open(path.join('info_diff', filename + '.json'), 'w+') as f:
        f.write(json.dumps(ret, sort_keys=True))
    return ret

pp(getInfoDiff('attribute', '1.16'))
pp(getInfoDiff('biome', '1.16'))
pp(getInfoDiff('block', '1.16'))
pp(getInfoDiff('dimension', '1.16'))
pp(getInfoDiff('effect', '1.16'))
pp(getInfoDiff('enchantment', '1.16'))
pp(getInfoDiff('entity', '1.16'))
pp(getInfoDiff('feature', '1.16'))
pp(getInfoDiff('fluid', '1.16'))
pp(getInfoDiff('item', '1.16'))
pp(getInfoDiff('tag/block', '1.16'))
pp(getInfoDiff('tag/entity_type', '1.16'))
pp(getInfoDiff('tag/fluid', '1.16'))
pp(getInfoDiff('tag/item', '1.16'))