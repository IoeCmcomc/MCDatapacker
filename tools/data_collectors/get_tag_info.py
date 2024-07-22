import json
from pathlib import Path

from commons import get_httpx_soup, find_tr_tags, get_html

soup = get_httpx_soup("https://minecraft.wiki/w/Tag?action=render")

h3 = soup.find("span", id="Java_Edition_2").parent
print(h3.name)

h4 = h3
while (h4 := h4.find_next_sibling("h4")) != None:
    h4_title = h4.span.text
    print(h4.name, h4_title)
    if h4_title.strip().casefold() == 'removed tags':
        break
    tag_category = h4_title.lower().replace(" ", "_")[:-5]
    info = dict()
    table = h4.find_next_sibling("table")
    tr_tags = table.find_all(find_tr_tags)
    if not tag_category in ('function_tags'):
        for tr_tag in tr_tags:
            td_tags = tr_tag.find_all("td")
            details = get_html(td_tags[1]).strip()
            info[td_tags[0].get_text(strip=True)] = details
        
        filename = "tag/" + tag_category + ".json"
        print(filename)
        Path("tag").mkdir(parents=True, exist_ok=True)
        with open(filename, "w+") as f:
            f.write(json.dumps({"added" : info}, sort_keys=True))