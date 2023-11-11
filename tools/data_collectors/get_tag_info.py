from bs4 import BeautifulSoup
import json
from pathlib import Path
from selenium import webdriver

driver = webdriver.ChromiumEdge()
driver.get("https://minecraft.wiki/wiki/Tag")
html = driver.page_source
driver.quit()
soup = BeautifulSoup(html, "lxml")

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
while (h4 := h4.find_next_sibling("h4")) != None:
    h4_title = h4.span.text
    print(h4.name, h4_title)
    if h4_title.strip().casefold() == 'removed tags':
        break
    tag_category = h4_title.lower().replace(" ", "_")[:-1] + ".json"
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