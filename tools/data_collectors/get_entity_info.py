import json

from commons import get_httpx_soup, find_tr_tags, get_html

ids_soup = get_httpx_soup("https://minecraft.wiki/w/Java_Edition_data_values/Entities")

info = dict()
tables = ids_soup.find_all("table")
for table in tables:
    tr_tags = table.find_all(find_tr_tags)
    for tr_tag in tr_tags:
        td_tags = tr_tag.find_all("td")
        name = td_tags[0].get_text().strip()
        id = td_tags[1].get_text(strip=True)
        info[id] = name

info['player'] = "Player"

with open("entity.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))
