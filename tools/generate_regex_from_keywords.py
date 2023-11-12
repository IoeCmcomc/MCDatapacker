from re import escape as re_escape

end = '\0'

strings = [
    "dir",
    "function",
    "block",
    "clock",
    "while",
    "async",
    "finally",
    "until",
    "else",
    "macro",
    "import",
    "!IF",
    "LOOP",
]

def escape(s: str) -> str:
    return re_escape(s.replace('/', r"\/"))

full_trie = {}
strings.sort()

for string in strings:
    trie = full_trie
    for c in string:
        if not (c in trie):
            trie[c] = {}
        trie = trie[c]
    trie[end] = None

print(full_trie)

def to_regex_str(trie: dict, depth: int = 0) -> str:
    output = ""
    if not trie:
        return output
    if len(trie) == 2 and end in trie:
        return "(?:" + next(escape(c) + to_regex_str(trie[c], depth+1) for c in trie if c != end) + ")?"
    if len(trie) > 1:
        output += "(?>"
    first = True
    for c, sub_trie in sorted(trie.items()):
        if first:
            first = False
        else:
            output += "|"
        if c != end:
            output += escape(c) + to_regex_str(sub_trie, depth+1)
    if len(trie) > 1:
        output += ")"
    return output


print(to_regex_str(full_trie).encode('latin-1', 'backslashreplace').decode('unicode-escape'))
