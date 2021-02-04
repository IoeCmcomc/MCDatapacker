from os import listdir
from os.path import exists, isfile, join, splitext, abspath
import re

path = abspath("../src")
for entry in listdir(path):
    entrypath = join(path, entry)
    
    if isfile(entrypath) and splitext(entry)[1].lower() == ".ui":
        print("File" if isfile(entrypath) else "Directory", '"'+entry+'"')
        with open(entrypath, "r+") as f:
            text = f.read()
            text = re.sub(r'src/([\w-]+\.h)', '\g<1>', text)
            f.seek(0)
            f.truncate()
            f.write(text)
    pass
                    