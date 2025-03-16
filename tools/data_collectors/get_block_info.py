import urllib.request
from bs4 import BeautifulSoup
import re
from PIL import Image, UnidentifiedImageError
from io import BytesIO
from pathlib import Path
from types import GeneratorType
import json
import httpx
from time import sleep
from commons import get_httpx_soup, get_image_online_httpx, find_tr_tags

def get_inv_icon_url(name: str, client: httpx.Client) -> str:
    api_url = "https://minecraft.wiki/api.php?"
    params = {
        'action': 'query',
        'format': 'json',
        'prop': 'imageinfo',
        'titles': f"File:Invicon {name}.png",
        'formatversion': 2,
        'iiprop': 'url'
        }
    res = client.get(api_url, params=params)
    # print(res.url, res.status_code)
    result_obj = res.json()["query"]["pages"][0]
    if 'missing' in result_obj:
        return ''
    return result_obj["imageinfo"][0]["url"]

ids_soup = get_httpx_soup('https://minecraft.wiki/w/Java_Edition_data_values/Blocks?action=render')

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
        icons.append((id, name, "https://minecraft.wiki" + icon['src'].replace("30px-", "300px-"), unobtainable))
        
    #print(icon_text)


Path("texture/inv_item").mkdir(parents=True, exist_ok=True)
Path("texture/block").mkdir(parents=True, exist_ok=True)
count = 0
with httpx.Client() as client:
    for element in icons:
        id = element[0]
        name = element[1]
        block_url = element[2]
        unobtainable = element[3]
        print(id, name, unobtainable)
        if unobtainable: # Unobtainable
            icon = get_image_online_httpx(block_url, client)
            print(block_url)
            icon.thumbnail((32, 32), resample=Image.Resampling.NEAREST)
            icon.save("texture/block/{}.png".format(id), optimize=True)
        else:
            # invicon_url = get_inv_icon_url(name, client)
            try:
                invicon_url = f"https://minecraft.wiki/images/Invicon_{name.replace(' ', '_')}.png"
                icon = get_image_online_httpx(invicon_url, client)
                print(invicon_url)
                icon = icon.resize((32, 32))
            except UnidentifiedImageError:
                icon = get_image_online_httpx(block_url, client)
                print(block_url)
                icon.thumbnail((32, 32), resample=Image.Resampling.NEAREST)
            icon.save("texture/inv_item/{}.png".format(id.lower()), optimize=True)
        count += 1
        print(f"{count=}")
        if count % 100 == 0:
            sleep(10)
        
with open("block.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))