import urllib.request
from bs4 import BeautifulSoup
import json
from pathlib import Path

req = urllib.request.urlopen('https://minecraft.fandom.com/wiki/Tag') 
soup = BeautifulSoup(req.read(), "html.parser")

h3 = soup.find("span", id="Java_Edition_2").parent
print(h3.name)

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.th is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag):
    html = str()
    for child in tag.children:
        html += str(child)
    return html

h4 = h3
while (h4 := h4.find_next_sibling("h4")) != None and h4.text != 'Removed Tags':
    print(h4.name, h4.text)
    tag_category = h4.span.text.lower().replace(" ", "_")[:-1] + ".json"
    info = dict()
    table = h4.find_next_sibling("table")
    tr_tags = table.find_all(find_tr_tags)
    if not tag_category in ('function.json'):
        for tr_tag in tr_tags:
            td_tags = tr_tag.find_all("td")
            details = get_html(td_tags[2]).strip()
            info[td_tags[0].get_text(strip=True)] = details
        
        filename = "tag/" + tag_category
        print(filename)
        Path("tag").mkdir(parents=True, exist_ok=True)
        with open(filename, "w+") as f:
            f.write(json.dumps({"added" : info}, sort_keys=True))