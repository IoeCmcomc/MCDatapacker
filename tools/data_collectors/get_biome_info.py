import urllib.request
from bs4 import BeautifulSoup
import json

req = urllib.request.urlopen('https://minecraft.wiki/w/Biome/ID') 
ids_soup = BeautifulSoup(req.read(), "lxml")

h1 = ids_soup.find("h1")

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.th is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag):
    html = str()
    for child in tag.children:
        #print(type(child))
        #print(child)
        html += str(child)
        #break
    return html

print(h1.name)
info = dict()
table = h1.find_next("table")
tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    name = td_tags[0].get_text(strip=True)
    id = td_tags[1].get_text(strip=True)
    info[id] = name
with open("biome.json", "w+") as f:
    f.write(json.dumps({"added" : info}))
