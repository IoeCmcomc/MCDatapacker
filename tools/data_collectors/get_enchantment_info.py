import json

from commons import get_httpx_soup, find_tr_tags

ids_soup = get_httpx_soup("https://minecraft.wiki/w/Java_Edition_data_values?action=render")

h3 = ids_soup.find("span", id="Enchantments").parent

print(h3.name)
info = dict()
table = h3.find_next_sibling("table")
tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    name = td_tags[0].get_text(strip=True)
    id = td_tags[1].get_text(strip=True)
    info[id] = {"name" : name}
with open("enchantment.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))
