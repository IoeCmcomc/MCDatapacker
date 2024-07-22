from urllib.request import Request, urlopen
from bs4 import BeautifulSoup, Tag
from PIL import Image
from io import BytesIO
from selenium import webdriver
import httpx
from typing import Optional

def get_soup(url: str) -> BeautifulSoup:
    req = Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    return BeautifulSoup(urlopen(req).read(), "lxml")

def get_selenium_soup(url: str) -> BeautifulSoup:
    driver = webdriver.ChromiumEdge()
    driver.get(url)
    soup = BeautifulSoup(driver.page_source, "lxml")
    driver.quit()
    return soup

def get_httpx_soup(url: str) -> BeautifulSoup:
    return BeautifulSoup(httpx.get(url).text, "lxml")

def get_image_online(url: str) -> Image:
    req = Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    with urlopen(req) as url:
        return Image.open(BytesIO(url.read()))

def get_image_online_httpx(url: str, client: Optional[httpx.Client] = None) -> Image:
    if client:
        return Image.open(BytesIO(client.get(url).content))
    else:
        return Image.open(BytesIO(httpx.get(url).content))

def find_tr_tags(tag):
    return (tag.name == "tr") and (tag.find('th', recursive=False) is None) \
           and (("upcoming" not in tag.find("td").text) or ("only" in tag.find("td").text))

def get_html(tag: Tag) -> str:
    html = str()
    for child in tag.children:
        html += str(child)
    return html
    