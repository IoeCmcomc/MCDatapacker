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


def get_icon_info(id: str, alt_name: str = None, name: str = None):
    global icons
    icon_text = icons_soup.find("li", text=id.replace('_', '-'))
    if (not icon_text) and name:
        icon_text = icons_soup.find("li", text=name.lower().replace(' ', '-'))
    if icon_text:
        icon = icon_text.parent.previous_sibling.find("span")
        pos_match = regex.search(icon['style'])
        pos = (int(i) for i in pos_match.groups())
        icons.append((alt_name if alt_name else id, pos,))


req = urllib.request.urlopen("https://minecraft.fandom.com/wiki/Java_Edition_data_values/Items")
ids_soup = BeautifulSoup(req.read(), "lxml")
req = urllib.request.urlopen('https://minecraft.fandom.com/wiki/Template:ItemSprite') 
icons_soup = BeautifulSoup(req.read(), "lxml")

regex = re.compile(r"-(\d+)px -(\d+)px")

info = dict()
icons = list()

table = ids_soup.find("table")
print(table.name)

tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    first_td = td_tags[0]
    name = first_td.get_text(strip=True)
    id = td_tags[1].get_text(strip=True)
    print(id, name)
    entry = {"name": name}
    info[id] = entry
    get_icon_info(id, name=name)
get_icon_info("banner_pattern")
get_icon_info("uncraftable-potion", "potion")
get_icon_info("uncraftable-splash-potion", "splash_potion")
get_icon_info("uncraftable-lingering-potion", "lingering_potion")

sheet_url = "https://static.wikia.nocookie.net/minecraft_gamepedia/images/f/f5/ItemCSS.png/revision/latest?cb=20230607112659"
with urllib.request.urlopen(sheet_url) as url:
    f = BytesIO(url.read())
sheet_img = Image.open(f)

Path("texture/item").mkdir(parents=True, exist_ok=True)
for element in icons:
    id = element[0]
    x, y = next(element[1]), next(element[1])
    print(id, x, y)
    icon = sheet_img.crop((x, y, x+16, y+16))
    icon.save("texture/item/{}.png".format(id))
    
with open("item.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))