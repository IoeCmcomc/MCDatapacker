import urllib.request
from bs4 import BeautifulSoup
import re
from PIL import Image
from io import BytesIO
from pathlib import Path
from types import GeneratorType
import json
import httpx
from commons import get_httpx_soup, get_image_online_httpx, find_tr_tags

ids_soup = get_httpx_soup('https://minecraft.wiki/w/Java_Edition_data_values/Blocks?action=render')

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
    icon = icon_td.find("img")
    if icon:
        icons.append((id, "https://minecraft.wiki" + icon['src'], unobtainable))
        
    #print(icon_text)


Path("texture/inv_item").mkdir(parents=True, exist_ok=True)
Path("texture/block").mkdir(parents=True, exist_ok=True)
with httpx.Client() as client:
    for element in icons:
        id = element[0]
        print(id, element[1], element[2])
        icon = get_image_online_httpx(element[1], client)
        icon.thumbnail((32, 32))
        if element[2]: # Unobtainable
            icon.save("texture/block/{}.png".format(id), optimize=True)
        else:
            icon.save("texture/inv_item/{}.png".format(id.lower()), optimize=True)
        
with open("block.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))