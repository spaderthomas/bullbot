import requests
from bs4 import BeautifulSoup
import re

battle_urls = []
for i in range(2):
    base_url = "https://replay.pokemonshowdown.com/search?user=&format=gen1ou&page=%i&output=html"
    r = requests.get(base_url % (i + 1)) # zero vs one indexing
    soup = BeautifulSoup(r.text)
    links = soup.find_all('a')
    for link in links:
        battle_urls.append(link.get('href'))

r = requests.get("https://replay.pokemonshowdown.com/gen1ou-607288101")
print(r.text)
