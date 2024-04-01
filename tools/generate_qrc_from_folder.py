from os import walk
from os.path import join, getsize, abspath, relpath
from json import dump as json_dump
import xml.etree.cElementTree as ET
from collections import namedtuple
from copy import deepcopy
from hashlib import md5
from functools import cache
from cbor2 import dump as cbor_dump

FileInfo = namedtuple('FileInfo', ('size', 'version',))

# Source: https://gist.github.com/tfeldmann/fc875e6630d11f2256e746f67a09c1ae
def chunk_reader(fobj, chunk_size=1024):
    """ Generator that reads a file in chunks of bytes """
    while True:
        chunk = fobj.read(chunk_size)
        if not chunk:
            return
        yield chunk

# Adapted from: https://gist.github.com/tfeldmann/fc875e6630d11f2256e746f67a09c1ae
@cache
def get_hash(filename):
    hashobj = md5()
    with open(filename, "rb") as f:
            hashobj.update(f.read(1024))
    return hashobj.digest()

def is_file_equal(path1: str, path2: str) -> bool:
    if get_hash(path1) != get_hash(path2):
        return False
    text1 = ""
    text2 = ""
    with open(path1, "r") as f:
        text1 = f.read()
    with open(path2, "r") as f:
        text2 = f.read()
    return text1 == text2

PREFIX_LEN = len("data/minecraft/")

def save_resource_lookup_file(version: str, info_dict: dict[str, FileInfo]) -> None:
    lookup_dict: dict[str, dict[str, str]] = {}

    for alias, file_info in info_dict.items():
        ver = file_info.version
        if (ver != version):
            key = alias[PREFIX_LEN:]
            category, key = key.split('/', 1)
            if category in ('tags', 'worldgen'):
                subcat, key = key.split('/', 1)
                category += '/' + subcat
            if category not in lookup_dict:
                lookup_dict[category] = {}
            lookup_dict[category][key] = ver
        
    json_path = join(abspath("../resource/minecraft/info"), version, f"vanilla_lookup.json")
    with open(json_path, 'w') as f:
        json_dump(lookup_dict, f, indent=2, sort_keys=True)
    cbor_path = join(abspath("../resource/minecraft/info"), version, f"vanilla_lookup.cbor")
    with open(cbor_path, 'wb') as f:
        cbor_dump(lookup_dict, f, canonical=True)

def generate_qrc(version: str, prev_ver: str = None, prev_info_dict: dict[str, FileInfo] = None, prev_aliases: set[str] = None) -> tuple[dict[str, FileInfo], set[str]]:
    print(f"Generating .qrc file for {version} vanilla datapack...")

    prefix = f"/minecraft/{version}/data-json"
    info_dir = abspath(f"../resource/minecraft/info") 
    qrc_dir = join(info_dir, version)
    # qrc_dir = abspath(f"../resource/minecraft/info")
    qrc_path = join(qrc_dir, f"{version}-data-json.qrc")
    target_dir = join(qrc_dir, "data-json")
    # target_dir = join(qrc_dir, f"{version}/data-json")

    if not prev_info_dict:
        info_dict = {}
    else:
        info_dict = deepcopy(prev_info_dict)
    aliases = set()

    added = 0
    updated = 0
    kept = 0
    removed = 0

    root = ET.Element("RCC")
    resource = ET.SubElement(root, "qresource", prefix=prefix)

    walk_iter = walk(target_dir)
    next(walk_iter) # Skip data folder
    for dirpath, dirnames, filenames in walk_iter:
        for filename in filenames:
            file_path = join(dirpath, filename)
            rel_path = relpath(file_path, qrc_dir).replace('\\', '/')
            alias_path = relpath(file_path, target_dir).replace('\\', '/')
            aliases.add(alias_path)
            file_size = getsize(file_path)
            # print(rel_path, alias_path)
            if not alias_path in info_dict:
                info_dict[alias_path] = FileInfo(file_size, version)
                ET.SubElement(resource, "file", alias=alias_path).text = rel_path
                added += 1
            elif info_dict[alias_path].size != file_size:
                info_dict[alias_path] = FileInfo(file_size, version)
                ET.SubElement(resource, "file", alias=alias_path).text = rel_path
                updated += 1
            elif prev_ver:
                    prev_path = join(info_dir, version, "data-json",  alias_path)
                    if not is_file_equal(file_path, prev_path):
                        ET.SubElement(resource, "file", alias=alias_path).text = rel_path
                        info_dict[alias_path] = FileInfo(file_size, version)
                        updated += 1
                    else:
                        kept += 1
    
    if prev_aliases:
        removed_files = prev_aliases - aliases
        for alias in removed_files:
            del info_dict[alias]
            removed += 1

    tree = ET.ElementTree(root)
    ET.indent(tree, space="\t", level=0)
    tree.write(qrc_path)

    with open(version + "-diff_info.json", 'w') as f:
        json_dump(info_dict, f, indent=4)

    save_resource_lookup_file(version, info_dict)

    print(f"{version} has {len(aliases)} file(s).")
    print(f"Added {added} file(s), updated {updated} file(s), kept {kept} file(s), removed {removed} file(s)")

    return info_dict, aliases

if __name__ == "__main__":
    version = "1.15"
    info, aliases = generate_qrc(version)
    prev_ver, version = version, "1.16"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.17"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.18"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.18.2"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.19"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.19.3"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.19.4"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.20"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.20.2"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.20.4"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
