from os import walk
from os.path import join, getsize, abspath, relpath
from json import dump as json_dump, JSONEncoder
from typing import ClassVar
import xml.etree.cElementTree as ET
from copy import deepcopy
from functools import cache, cached_property
from cbor2 import dump as cbor_dump
from dataclasses import dataclass
from xxhash import xxh64
from jellyfish import levenshtein_distance

# Source: https://gist.github.com/tfeldmann/fc875e6630d11f2256e746f67a09c1ae
def chunk_reader(fobj, chunk_size=1024):
    """ Generator that reads a file in chunks of bytes """
    while True:
        chunk = fobj.read(chunk_size)
        if not chunk:
            return
        yield chunk

@dataclass(frozen=True)
class File:
    version: str
    path: str
    rel_path: str
    alias: str

    partial_hash_count: ClassVar[int] = 0
    size_count: ClassVar[int] = 0
    data_count: ClassVar[int] = 0
    
    @classmethod
    @cache
    # Adapted from: https://gist.github.com/tfeldmann/fc875e6630d11f2256e746f67a09c1ae
    def __hash_from_path(cls, path) -> int:
        # print(f"hashing {path}")
        hashobj = xxh64()
        if path:
            with open(path, "rb") as f:
                for chunk in chunk_reader(f):
                    hashobj.update(chunk)
        return hashobj.intdigest()

    def __hash__(self) -> int:
        return self.__hash_from_path(self.path)
    
    @cached_property
    def partial_hash(self) -> int:
        self.__class__.partial_hash_count += 1
        hashobj = xxh64()
        if self.path:
            with open(self.path, "rb") as f:
                head_part = f.read(1024)
                hashobj.update(head_part)
        return hashobj.digest()
    
    @cached_property
    def data(self) -> bytes:
        self.__class__.data_count += 1

        result = bytes()
        if self.path:
            with open(self.path, "rb") as f:
                result = f.read()
        
        return result
    
    @cached_property
    def size(self) -> int:
        self.__class__.size_count += 1
        return getsize(self.path)
    
    def __eq__(self, other: object) -> bool:
        assert isinstance(other, File)
        if other is self:
            return True
        if self.size != other.size:
            return False
        elif self.partial_hash != other.partial_hash:
            return False
        return self.data == other.data
    
    def __lt__(self, other) -> bool:
        assert isinstance(other, File)
        return self.path > other.path

class CustomJSONEncoder(JSONEncoder):
        def default(self, o):
            if isinstance(o, File):
                return {'version': o.version, 'alias': o.alias}
            return super().default(o)

def paths_to_dict(paths: dict[str, str]) -> dict:
    folders = {}
    for path, ver in paths.items():
        parts = path.split('/')
        last_part = parts[-1]
        cur_dir = folders
        for part in parts[:-1]:
            cur_dir = cur_dir.setdefault(part, {})
        cur_dir[last_part] = ver

    return folders

def extract_category(path: str) -> str:
    category, path = path.split('/', 1)
    if category in ('tags', 'worldgen'):
        subcat, path = path.split('/', 1)
        category += '/' + subcat
    return category, path

PREFIX_LEN = len("data/minecraft/")
INITIAL_FILE_PATH_DISTANCE = 123456

@dataclass(frozen=True)
class FileAlias:
    alias: str
    file: File

    def __eq__(self, value: object) -> bool:
        return self.alias == value.alias
    
    def __hash__(self) -> int:
        return hash(self.alias)


def save_resource_lookup_file(version: str, info_dict: dict[str, File], categories: set[str] = None) -> None:
    lookup_dict: dict[str, dict[str, str]] = {}

    for alias, file_info in info_dict.items():
        ver = file_info.version
        if (ver != version):
            key = alias[PREFIX_LEN:]
            category, key = extract_category(key)
            if category not in lookup_dict:
                lookup_dict[category] = {}
            if alias != file_info.alias:
                # print(f"{category=}, {key=}, {file_info.alias=}")
                lookup_dict[category][key] = f"{ver}/data-json/{file_info.alias}"
            else:
                lookup_dict[category][key] = ver
    
    # for category, aliases in lookup_dict.items():
    #     lookup_dict[category] = paths_to_dict(aliases)
    for category in categories:
        if category not in lookup_dict:
            lookup_dict[category] = {}
        
    json_path = join(abspath("../res/mc/info"), version, f"vanilla_lookup.json")
    with open(json_path, 'w') as f:
        json_dump(lookup_dict, f, indent=1, sort_keys=True)
    cbor_path = join(abspath("../res/mc/info"), version, f"vanilla_lookup.cbor")
    with open(cbor_path, 'wb') as f:
        cbor_dump(lookup_dict, f, canonical=True)

def generate_qrc(version: str, prev_ver: str = None, prev_info_dict: dict[str, File] = None, prev_aliases: set[FileAlias] = None) -> tuple[dict[str, File], set[FileAlias]]:    
    print(f"Generating .qrc file for {version} vanilla datapack...")

    prefix = f"/minecraft/{version}/data-json"
    info_dir = abspath(f"../res/mc/info") 
    qrc_dir = join(info_dir, version)
    # qrc_dir = abspath(f"../res/mc/info")
    qrc_path = join(qrc_dir, f"{version}-data-json.qrc")
    target_dir = join(qrc_dir, "data-json")
    minecraft_dir = join(target_dir, "data", "minecraft")
    # target_dir = join(qrc_dir, f"{version}/data-json")

    if not prev_info_dict:
        info_dict = {}
    else:
        info_dict = deepcopy(prev_info_dict)
    aliases: set[FileAlias] = set()
    categories: set[str] = set()

    added = 0
    updated = 0
    kept = 0
    removed = 0
    moved = 0

    root = ET.Element("RCC")
    resource = ET.SubElement(root, "qresource", prefix=prefix)
    included_files: dict[str, File] = {}

    walk_iter = walk(target_dir)
    next(walk_iter) # Skip data folder
    for dirpath, dirnames, filenames in walk_iter:
        for filename in sorted(filenames):
            file_path = join(dirpath, filename)
            rel_path = relpath(file_path, qrc_dir).replace('\\', '/')
            alias_path = relpath(file_path, target_dir).replace('\\', '/')
            id_path = relpath(file_path, minecraft_dir).replace('\\', '/')
            file_info = File(version, file_path, rel_path, alias_path)
            aliases.add(FileAlias(alias_path, file_info))
            category, _ = extract_category(id_path)
            categories.add(category)
            file_size = getsize(file_path)
            # print(rel_path, alias_path)
            if not alias_path in info_dict:
                info_dict[alias_path] = file_info
                included_files[alias_path] = file_info
                # add_elem()
                added += 1
            elif info_dict[alias_path].size != file_size:
                info_dict[alias_path] = file_info
                included_files[alias_path] = file_info
                # add_elem()
                updated += 1
            elif prev_ver:
                    prev_file = info_dict[alias_path]
                    if not file_info == prev_file:
                        # add_elem()
                        included_files[alias_path] = file_info
                        info_dict[alias_path] = file_info
                        updated += 1
                    else:
                        kept += 1
    
    if prev_aliases:
        removed_files = sorted(list(file.file for file in (prev_aliases - aliases)))
        added_files = sorted(list(file.file for file in (aliases - prev_aliases)))

        report_lines = []

        for i in range(len(removed_files) - 1, -1, -1):
            removed_file = removed_files[i]
            # if removed_file.alias.endswith("hanging_signs.json"):
            #     print(f"{removed_file.alias}")
            the_added_file = None
            ii = 0
            jj = 0
            added_file_path_distance = INITIAL_FILE_PATH_DISTANCE
            for j in range(len(added_files) - 1, -1, -1):
                added_file = added_files[j]
                if removed_file == added_file:
                    path_distance = levenshtein_distance(removed_file.alias, added_file.alias)
                    # if removed_file.alias.endswith("hanging_signs.json"):
                    #     print(f"{added_file.alias=}, {path_distance=}, {added_file_path_distance=}")
                    if path_distance < added_file_path_distance:
                        added_file_path_distance = path_distance
                        the_added_file = added_file
                        ii = i
                        jj = j

            # if removed_file.alias.endswith("hanging_signs.json"):
            #     print(f"{the_added_file.alias=}")

            if isinstance(the_added_file, File):
                old_file: File = info_dict[removed_file.alias]
                info_dict[the_added_file.alias] = old_file
                del info_dict[removed_file.alias]
                report_lines.append(f"Found a moved file!")
                report_lines.append(f"\tfrom: {old_file.path}")
                report_lines.append(f"\tto: {the_added_file.path}")
                del removed_files[ii]
                del added_files[jj]
                del included_files[the_added_file.alias]
                moved += 1        
                
        print(f"{File.data_count=}, {File.partial_hash_count=}, {File.size_count=}")

        with open(f"report_{prev_ver}_{version}.txt", "w") as f:
            f.write('\n'.join(report_lines))

        for alias in removed_files:
            del info_dict[alias.alias]
            removed += 1

    added -= moved

    for alias, file in sorted(included_files.items()):
        elem = ET.SubElement(resource, "file", alias=file.alias)
        elem.text = file.rel_path

    tree = ET.ElementTree(root)
    ET.indent(tree, space="\t", level=0)
    tree.write(qrc_path)

    with open(version + "-diff_info.json", 'w') as f:
        json_dump(info_dict, f, indent=4, cls=CustomJSONEncoder)

    save_resource_lookup_file(version, info_dict, categories)

    print(f"{version} has {len(aliases)} file(s).")
    print(f"This version has {len(categories)} categories.")
    print(f"Added {added} file(s), updated {updated} file(s), kept {kept} file(s), removed {removed} file(s), moved {moved} file(s)")

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
    prev_ver, version = version, "1.20.6"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.21"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.21.3"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
    prev_ver, version = version, "1.21.4"
    info, aliases = generate_qrc(version, prev_ver, info, aliases)
