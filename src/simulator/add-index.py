import json
from pprint import pprint

with open('type-chart.json') as file:
    pokemonData = file.read()
    pokemonData = json.loads(pokemonData)
    
    indx = 0
    for pokemon in pokemonData:
        pokemonData[pokemon]["index"] = indx
        indx = indx + 1


with open('type-chart.json', 'w') as outfile:
    json.dump(pokemonData, outfile)

