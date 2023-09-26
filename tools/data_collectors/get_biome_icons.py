import urllib.request
from bs4 import BeautifulSoup
import re
from PIL import Image
from io import BytesIO
from pathlib import Path

req = urllib.request.urlopen('https://minecraft.wiki/w/Biome/ID') 
ids_soup = BeautifulSoup(req.read(), "html.parser")

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
info = list()
table = h1.find_next("table")
tr_tags = table.find_all(find_tr_tags)

regex = re.compile(r"-(\d+)px -(\d+)px")

for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    id = td_tags[1].get_text(strip=True)
    pos_match = regex.search(td_tags[0].find("span", class_="sprite")['style'])
    pos = (int(i) for i in pos_match.groups())
    info.append((id, pos,))

sheet_url = "https://static.wikia.nocookie.net/minecraft_gamepedia/images/5/59/BiomeCSS.png"
with urllib.request.urlopen(sheet_url) as url:
    f = BytesIO(url.read())
sheet_img = Image.open(f)

Path("texture/biome").mkdir(parents=True, exist_ok=True)
for element in info:
    id = element[0]
    x, y = next(element[1]), next(element[1])
    print(id, x, y)
    icon = sheet_img.crop((x, y, x+16, y +16))
    icon.save("texture/biome/{}.png".format(id))

# with open("biome.json", "w+") as f:
    # f.write(json.dumps({"added" : info}))
    
# https://gamepedia.cursecdn.com/minecraft_gamepedia/5/59/BiomeCSS.png