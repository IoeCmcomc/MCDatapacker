import urllib.request
from bs4 import BeautifulSoup
import json
import re

req = urllib.request.urlopen('https://minecraft.gamepedia.com/Java_Edition_data_values') 
ids_soup = BeautifulSoup(req.read(), "html.parser")

h3 = ids_soup.find("span", id="Enchantments").parent

req = urllib.request.urlopen('https://minecraft.gamepedia.com/Enchanting')
sums_soup = BeautifulSoup(req.read(), "html.parser")

sum_table = sums_soup.find("table", {"data-description": "Summary of enchantments"})

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.th is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag):
    html = str()
    for child in tag.children:
        #print(type(child))
        #print(child)
        html += str(child)
        #break
    return html

print(h3.name)
info = dict()
table = h3.find_next_sibling("table")
tr_tags = table.find_all(find_tr_tags)
for tr_tag in tr_tags:
    td_tags = tr_tag.find_all("td")
    name = td_tags[0].get_text(strip=True)
    id = td_tags[1].get_text(strip=True)
    sum_tr = sum_table.find(lambda tr: find_tr_tags(tr) and (name in tr.find("td").text))
    summary = get_html(sum_tr.find_all("td")[1])
    info[id] = {"name" : name, "summary" : summary}
with open("enchantment.json", "w+") as f:
    f.write(json.dumps({"added" : info}))
