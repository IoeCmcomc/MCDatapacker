import urllib.request
from bs4 import BeautifulSoup
import json

req = urllib.request.urlopen('https://minecraft.wiki/w/Attribute')
ids_soup = BeautifulSoup(req.read(), "html.parser")

info = dict()

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.find('th', recursive=False) is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag):
    html = str()
    for child in tag.children:
        html += str(child)
    return html

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
        
get_per_h3("Attributes_available_on_all_living_entities")
get_per_h3("Attributes_for_players")
get_per_h3("Attributes_for_horses")
get_per_h3("Attributes_for_bees_and_parrots")
get_per_h3("Attributes_for_zombies")

with open("attribute.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))
