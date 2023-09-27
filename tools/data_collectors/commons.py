from urllib.request import Request, urlopen
from bs4 import BeautifulSoup, Tag

def get_soup(url: str) -> BeautifulSoup:
    req = Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    return BeautifulSoup(urlopen(req).read(), "lxml")

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.find('th', recursive=False) is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag: Tag) -> str:
    html = str()
    for child in tag.children:
        html += str(child)
    return html