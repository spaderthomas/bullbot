import requests
from unidecode import unidecode
from bs4 import BeautifulSoup
import re

def cleanLog(log):
    # Remove everything before the start (log starts on line 17) except player names (which are on lines 4 and 5)
    usefulLines = open(log, "rw").readlines()
    names = [x for x in usefulLines if re.match('\|player', x)]
    usefulLines = usefulLines[15:]
    # Remove all the following useless lines
    usefulLines = names + [l for l in usefulLines if not (re.match('\|c\|', l) or re.match('\|J\|', l) or re.match('\|upkeep', l) or re.match('\|L\|', l) or re.match('\|-crit', l) or re.match('\|-resisted', l) or re.match('\|-fail', l) or re.match('\|cant', l) or re.match('\|-supereffective', l))]
    # Add newlines (but only between each turn) so we can delimit turns
    keep = True
    delete = []
    for i in range(len(usefulLines)):
        if usefulLines[i] == '|\n':
            if keep:
                usefulLines[i] = '\n'
                keep = False
            else:
                delete.append(i)
                keep = True 
    usefulLines = [usefulLines[i] for i in range(len(usefulLines)) if i not in delete]
    # Add a new line to delimit turn 0, which occurs at line 7
    usefulLines.insert(6, '\n')
    open(log, "w").close()
    with open(log, "w") as file:
        file.writelines(usefulLines)
    
    

def gatherLogs():
    baseURL = "http://replay.pokemonshowdown.com/"
    replays = requests.get(baseURL)
    replays = unidecode(replays.text)
    replays = BeautifulSoup(replays, 'html.parser')

    # Find end of URL of all battles which match regex
    validBattles = []
    for replay in replays.find_all('a'):
        if re.search('/gen1ou', replay['href']):
            validBattles.append(replay['href'])

    print validBattles

    # Go to each replay and write the log to a text file
    for battle in validBattles:
        battleURL = battle[1:]+".txt"
        battle = requests.get(baseURL + battle)
        battle = unidecode(battle.text)
        battle = BeautifulSoup(battle, 'html.parser')
        for tag in battle.find_all('script'):
            if tag.get('class') == [u'log']:
                with open(battleURL, "a+") as file:
                    file.write(str(tag))
                cleanLog(battleURL)

cleanLog("test-log.txt")
