from pathlib import Path
import json
import httpx

from commons import find_tr_tags, get_httpx_soup, get_image_online_httpx

def get_icon_info(id: str, alt_name: str = None, name: str = None):
    global client
    api_url = "https://minecraft.wiki/api.php?"
    icon_id = id.replace('_', '-')
    params = {
        'action': 'query',
        'format': 'json',
        'prop': 'imageinfo',
        'titles': f"File:ItemSprite_{icon_id}.png",
        'formatversion': 2,
        'iiprop': 'url'
        }
    res = client.get(api_url, params=params)
    print(res.url, res.status_code)
    img_url = res.json()["query"]["pages"][0]["imageinfo"][0]["url"]

    # if (not img_url) and name:
    #     icon_text = icons_soup.find("code", string=name.lower().replace(' ', '-'))
    if img_url:
        icons.append((alt_name if alt_name else id, img_url,))

with httpx.Client() as client:
    ids_soup = get_httpx_soup("https://minecraft.wiki/w/Java_Edition_data_values/Items?action=render")

    info: dict[str, dict] = {}
    icons: list[tuple[str, str]] = []

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
        img = first_td.find("img")
        if img:
            icons.append((id, "https://minecraft.wiki" + img['src']))
    get_icon_info("banner_pattern")
    get_icon_info("uncraftable-potion", "potion")
    get_icon_info("uncraftable-splash-potion", "splash_potion")
    get_icon_info("uncraftable-lingering-potion", "lingering_potion")

    Path("texture/item").mkdir(parents=True, exist_ok=True)
    for element in icons:
        id = element[0]
        url = element[1]
        print(id, url)
        icon = get_image_online_httpx(url, client)
        icon.save("texture/item/{}.png".format(id))
        
    with open("item.json", "w+") as f:
        f.write(json.dumps({"added" : info}, sort_keys=True))