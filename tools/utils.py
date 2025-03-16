from os import path
import json

INFO_PATH = "../res/mc/info"

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