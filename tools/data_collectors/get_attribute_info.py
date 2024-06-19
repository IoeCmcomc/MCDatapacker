from bs4 import BeautifulSoup
import json

from commons import get_selenium_soup, find_tr_tags

ids_soup = get_selenium_soup("https://minecraft.wiki/w/Attribute?oldid=2600109")

info = dict()

def get_per_h3(h3_id):
    h3 = ids_soup.find("span", id=h3_id).parent
    print(h3.name)
    global info
    table = h3.find_next_sibling("table")
    tr_tags = table.tbody.find_all(find_tr_tags, recursive=False)
    for tr_tag in tr_tags:
        td_tags = tr_tag.find_all("td")
        name = td_tags[0].get_text(strip=True)
        if '.' in name:
            info[name] = None

def get_per_h2(h2_id):
    h2 = ids_soup.find("span", id=h2_id).parent
    print(h2.name)
    global info
    table = h2.find_next_sibling("table")
    tr_tags = table.tbody.find_all(find_tr_tags, recursive=False)
    for tr_tag in tr_tags:
        td_tags = tr_tag.find_all("td")
        name = td_tags[0].get_text(strip=True)
        if '.' in name:
            info[name] = None

        
# get_per_h3("Attributes_available_on_all_living_entities")
# get_per_h3("Attributes_for_players")
# get_per_h3("Attributes_for_horses")
# get_per_h3("Attributes_for_bees_and_parrots")
# get_per_h3("Attributes_for_zombies")
get_per_h3("Attributes")

with open("attribute.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))
