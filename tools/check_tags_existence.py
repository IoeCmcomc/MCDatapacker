from os import path, scandir
from pathlib import Path
import json
from pprint import pp

from utils import INFO_PATH

def verify_ids_existence(category: str, version: str):
    print(f"Verifying IDs in {category} category of version {version}")

    registry: list = []
    if category in {'tag/biome', 'tag/flat_level_generator_preset', 'tag/structure', 'tag/world_preset'}:
        registry_category = 'tag/worldgen/' + category.split('/')[-1]
    elif category == 'entity':
        registry_category = 'entity_type'
    else:
        registry_category = category
    with open(path.join(INFO_PATH, version, 'registries', registry_category, 'data.min.json')) as f:
        registry = json.load(f)

    info: dict = {}
    with open(path.join('data_collectors', category + '.json')) as f:
        info = json.load(f)

    for id in registry:
        id: str
        if id not in info['added']:
            print(f'\tMissing ID in {category} info: {id}')

    for id in info['added'].keys():
        id: str
        if id not in registry:
            print(f'\tMissing ID in {category} registry: {id}')

def verify_items_existence(version: str):
    print(f"Verifying items of version {version}")

    registry: list = []
    with open(path.join(INFO_PATH, version, 'registries', 'item', 'data.min.json')) as f:
        registry = json.load(f)

    info: dict = {}
    with open(path.join('data_collectors', 'item.json')) as f:
        info = json.load(f)

    block_info: dict = {}
    with open(path.join('data_collectors', 'block.json')) as f:
        block_info = json.load(f)
    for id, props in block_info['added'].items():
        if ('unobtainable' not in props) and (id not in info):
            info['added'][id] = props    

    for id in registry:
        id: str
        if id not in info['added']:
            print(f'\tMissing item in info: {id}')

    for id in info['added'].keys():
        id: str
        if id not in registry:
            print(f'\tMissing item in registry: {id}')

def verify_tags_existence(version: str):
    subfolders = [ Path(f.path).stem for f in scandir(path.join('data_collectors', 'tag')) if f.is_file() ]
    for folder in subfolders:
        verify_ids_existence('tag/' + folder, version)

if __name__ == '__main__':
    version = '1.21.4'
    verify_ids_existence('block', version)
    verify_ids_existence('entity', version)
    verify_items_existence(version)
    verify_tags_existence(version)