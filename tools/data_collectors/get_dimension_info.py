import urllib.request
from bs4 import BeautifulSoup
import json

req = urllib.request.urlopen('https://minecraft.wiki/w/Java_Edition_data_values') 
ids_soup = BeautifulSoup(req.read(), "html.parser")

h3 = ids_soup.find("span", id="Dimensions").parent

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.th is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag):
    html = str()
    for child in tag.children:
        html += str(child)
    return html

print(h3.name)
info = dict()
table = h3.find_next_sibling("table")
tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    name = td_tags[0].get_text(strip=True)
    id = td_tags[1].get_text(strip=True)
    info[id] = name
with open("dimension.json", "w+") as f:
    f.write(json.dumps({"added" : info}, sort_keys=True))
