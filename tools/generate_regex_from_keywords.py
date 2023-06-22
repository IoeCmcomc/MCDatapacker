strings = [
    "advancement",
    "attribute",
    "ban",
    "ban-ip",
    "banlist",
    "bossbar",
    "clear",
    "clone",
    "data",
    "datapack",
    "debug",
    "defaultgamemode",
    "deop",
    "difficulty",
    "effect",
    "echant",
    "execute",
    "experience",
    "fill",
    "forceload",
    "function",
    "gamemode",
    "gamerule",
    "give",
    "help",
    "item",
    "jfr",
    "kick",
    "kill",
    "list",
    "locate",
    "locatebiome",
    "loot",
    "me",
    "msg",
    "op",
    "pardon",
    "pardon-ip",
    "particle",
    "perf",
    "playsound",
    "publish",
    "recipe",
    "reload",
    "replaceitem",
    "save-all",
    "save-off",
    "save-on",
    "say",
    "schedule",
    "scoreboard",
    "seed",
    "setblock",
    "setidletimeout",
    "setworldspawn",
    "spawnpoint",
    "spectate",
    "spreadplayer",
    "stop",
    "stopsound",
    "summon",
    "tag",
    "team",
    "teleport",
    "teammsg",
    "tell",
    "tellraw",
    "time",
    "title",
    "tm",
    "tp",
    "trigger",
    "w",
    "weather",
    "whitelist",
    "worldborder",
    "xp",
]

full_trie = {}

for string in strings:
    trie = full_trie
    for c in string:
        if not (c in trie):
            trie[c] = {}
        trie = trie[c]
    trie['~'] = None

def to_regex_str(trie: dict, depth: int = 0) -> str:
    output = ""
    if not trie:
        return output
    if len(trie) == 2 and '~' in trie:
        return "(?:" + next(c + to_regex_str(trie[c], depth+1) for c in trie if c != '~') + ")?"
    if len(trie) > 1:
        output += "(?>"
    first = True
    for c, sub_trie in sorted(trie.items()):
        if first:
            first = False
        else:
            output += "|"
        if c != '~':
            output +=c + to_regex_str(sub_trie, depth+1)
    if len(trie) > 1:
        output += ")"
    return output


print(to_regex_str(full_trie))
