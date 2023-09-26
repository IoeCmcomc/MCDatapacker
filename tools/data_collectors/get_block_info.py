import urllib.request
from bs4 import BeautifulSoup
import re
from PIL import Image
from io import BytesIO
from pathlib import Path
from types import GeneratorType
import json

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.th is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

req = urllib.request.urlopen('https://minecraft.wiki/w/Java_Edition_data_values/Blocks') 
ids_soup = BeautifulSoup(req.read(), "lxml")
req = urllib.request.urlopen('https://minecraft.wiki/w/Template:InvSprite') 
icons_soup = BeautifulSoup(req.read(), "lxml")

regex = re.compile(r"-(\d+)px -(\d+)px")

info = dict()
icons = list()

table = ids_soup.find_all("table")[1]

tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    icon_td = td_tags[0]
    id_td = td_tags[1]
    name_td = td_tags[2]
    unobtainable = td_tags[3].has_attr("style")
    name = name_td.get_text(strip=True)
    id = id_td.get_text(strip=True)
    # print(id, name, unobtainable)
    entry = {"name": name}
    if unobtainable:
        entry["unobtainable"] = True
    info[id] = entry
    icon_text = icons_soup.find("li", text=name)
    if icon_text:
        icon = icon_text.parent.previous_sibling.find("span")
        #print(icon)
        pos_match = regex.search(icon['style'])
        pos = (int(i) for i in pos_match.groups())
        icons.append((id, pos,))
    else:
        icon = icon_td.find("img")
        if icon:
            icon_src: str = icon["data-src"];
            icon_src = re.sub(r"/scale-to-width-down/.+", "", icon_src)
            icons.append((id, icon_src,))
        
    #print(icon_text)

sheet_url = "https://static.wikia.nocookie.net/minecraft_gamepedia/images/4/44/InvSprite.png"
with urllib.request.urlopen(sheet_url) as url:
    f = BytesIO(url.read())
sheet_img = Image.open(f)

Path("texture/inv_item").mkdir(parents=True, exist_ok=True)
Path("texture/block").mkdir(parents=True, exist_ok=True)
for element in icons:
    id = element[0]
    if isinstance(element[1], GeneratorType):
        x, y = next(element[1]), next(element[1])
        print(id, x, y)
        icon = sheet_img.crop((x, y, x+32, y +32))
        icon.save("texture/inv_item/{}.png".format(id.lower()), optimize=True)
    else:
        print(id, element[1])
        with urllib.request.urlopen(element[1]) as response:
            icon = Image.open(BytesIO(response.read()))
            icon.thumbnail((32, 32))
            icon.save("texture/block/{}.png".format(id), optimize=True)
    
with open("block.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))