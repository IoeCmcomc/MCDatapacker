import urllib.request
from bs4 import BeautifulSoup
import json
from pathlib import Path

req = urllib.request.urlopen('https://minecraft.gamepedia.com/Tag') 
soup = BeautifulSoup(req.read(), "html.parser")

h2 = soup.find("span", id="List_of_tags").parent
print(h2.name)

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

h3 = h2
while (h3 := h3.find_next_sibling("h3")) != None:
    print(h3.name)
    info = dict()
    table = h3.find_next_sibling("table")
    tr_tags = table.find_all(find_tr_tags)
    for tr_tag in tr_tags:
        td_tags = tr_tag.find_all("td")
        details = get_html(td_tags[2]).strip()
        info[td_tags[0].get_text(strip=True)] = details
    filename = "tag/" + h3.span.text.lower().replace(" ", "_")[:-1] + ".json"
    print(filename)
    Path("tag").mkdir(parents=True, exist_ok=True)
    with open(filename, "w+") as f:
        f.write(json.dumps({"added" : info}))
