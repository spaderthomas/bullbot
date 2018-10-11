import requests
from bs4 import BeautifulSoup
import re
import pprint
from dbutils import *
import copy

printer = pprint.PrettyPrinter(indent=4)

# Throw out any matches which contain nonviable Pokemon 
legal_pokemon = ["alakazam", "articuno", "chansey", "cloyster", "dragonite", "exeggutor", "gengar", "golem", "hypno", "jolteon", "jynx", "lapras", "persian", "rhydon", "slowbro", "snorlax", "starmie", "tauros", "victreebel", "zapdos"]

# Converts a list of player switches into all revealed Pokemon
def get_team(switches):
    discard = len("|switch|pxa: ")
    team = []
    for switch in switches:
        switch = switch[discard:]
        pokemon = switch.split('|')[1].lower()
        if pokemon not in team:
            team.append(pokemon)
    return team


# Parse the PS replay site for replay URLs
battle_urls = [] # e.g. /gen1ou-607288101
num_replay_pages = 2
for i in range(num_replay_pages):
    base_url = "https://replay.pokemonshowdown.com/search?user=&format=gen1ou&page=%i&output=html"
    r = requests.get(base_url % (i + 1)) # zero vs one indexing
    soup = BeautifulSoup(r.text)
    links = soup.find_all('a')
    for link in links:
        battle_urls.append(link.get('href'))

#battle_urls = ['/gen1ou-818756174'] # for testing!

# Go through all the URLs and parse data
for battle_url_postfix in battle_urls:
    # Don't add the samples if we've already seen the battle
    battle_id = int(battle_url_postfix.split("-")[-1]) # e.g. 607288101
    if already_have_battle(battle_id):
        break

    # Get the raw battle text
    url = "https://replay.pokemonshowdown.com" + battle_url_postfix
    r = requests.get(url)

    # Parse out the lines which contain switch information
    switches = [[line for line in r.text.split("\n") if 'switch|p1' in line], \
                [line for line in r.text.split("\n") if 'switch|p2' in line]]
    # Construct list of all revealed Pokemon for each player
    full_teams = [get_team(switch_list) for switch_list in switches]

    # Construct samples as Pokemon are revealed
    for iplayer in range(2):
        full_team = full_teams[iplayer]
        revealed = []
        samples = []
        valid = True
        for switch in switches[iplayer]:
            # Each switch is like '|switch|p2a: NICKNAME|Dragonite|385\\/385'
            # Extract the Pokemon name from the switch
            discard = len("|switch|pxa: ")
            switch = switch[discard:]
            pokemon = switch.split('|')[1].lower()
            if pokemon not in legal_pokemon:
                print("Invalid Pokemon: %s" % pokemon)
                valid = False
                break
            
            # If it reveals a new Pokemon but does not complete team, add a new sample
            if pokemon not in revealed:
                if len(revealed) != len(full_team) - 1:
                    revealed.append(pokemon)
                    samples.append(copy.deepcopy(revealed))

        # Handle nonviable Pokemon by just not including the player
        if not valid:
            continue

        #insert(samples, full_team, battle_id)
