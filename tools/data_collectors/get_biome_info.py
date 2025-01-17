import json
from pathlib import Path
import httpx

from commons import get_httpx_soup, get_image_online_httpx

ids_soup = get_httpx_soup('https://minecraft.wiki/w/Biome/ID') 

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
biome_icons = dict()
table = h1.find_next("table")
tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    name = td_tags[0].get_text(strip=True)
    icon = td_tags[0].find('img')['src']
    id = td_tags[1].get_text(strip=True)
    info[id] = name
    biome_icons[id] = 'https://minecraft.wiki' + icon

Path("texture/biome").mkdir(parents=True, exist_ok=True)
client = httpx.Client()
for id, url in biome_icons.items():
    print(id, url)
    icon = get_image_online_httpx(url, client)
    icon.save(f"texture/biome/{id}.png", optimize=True)

with open("biome.json", "w+") as f:
    f.write(json.dumps({"added" : info}))
