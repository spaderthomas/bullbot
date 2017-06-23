import re
import json
import pprint as pp

# Consult documentation (README.md) for format of game output
def parseLog(filename):
    game = {'players': {}, 'turns': []}
    with open(filename, "r") as logfile:
        log = logfile.readlines()
    # Delimit on |
    log = [i.split("|") for i in log]
    # Remove empty string elements and separate atomics into the move they belong
    turns = []
    turn = []
    for i in range(len(log)):
        log[i] = [x for x in log[i] if x != '']
        log[i][-1] = log[i][-1][:-1] # kill newlines
        if (log[i] == ['']): # moves delimited by '' where newlines were
            turns.append(turn)
            turn = []
            continue
        turn.append(log[i])
    turns.append(log[len(log)-1]) # this misses the win declaration
    # Create mapping from players to names and then delete this move
    game['players']['p1a'] = turns[0][0][2]
    game['players']['p2a'] = turns[0][1][2]
    del(turns[0])
    ''' Parse batle information. At this point the atomics (i.e. move[i]) look like this:
            ['switch', 'p1a: Alakazam', 'Alakazam', '313\\/313']
            ['switch', 'p2a: Alakazam', 'Alakazam', '313\\/313']
            ['move', 'p1a: Alakazam', 'Thunder Wave', 'p2a: Alakazam']
            ['-status', 'p2a: Alakazam', 'par']
            ['move', 'p1a: Alakazam', 'Thunder Wave', 'p2a: Alakazam']
            ['move', 'p2a: Alakazam', 'Psychic', 'p1a: Alakazam']
            ['-damage', 'p1a: Alakazam', '264\\/313']
            ['move', 'p1a: Alakazam', 'Recover', 'p1a: Alakazam']
            ['-heal', 'p1a: Alakazam', '313\\/313']
            ['move', 'p2a: Alakazam', 'Psychic', 'p1a: Alakazam']
            ['-damage', 'p1a: Alakazam', '257\\/313']
            ['switch', 'p1a: Tauros', 'Tauros', '353\\/353']
            ['move', 'p2a: Alakazam', 'Psychic', 'p1a: Tauros']
            ['-damage', 'p1a: Tauros', '23\\/353']
            ['move', 'p1a: Tauros', 'Blizzard', 'p2a: Alakazam']
            ['-damage', 'p2a: Alakazam', '170\\/313 par']
            ['move', 'p2a: Alakazam', 'Psychic', 'p1a: Tauros']
            ['-damage', 'p1a: Tauros', '0 fnt']
            ['faint', 'p1a: Tauros']
            ['-message', 'bad ass forfeited.']
    '''
    turnNum = 1
    for turn in turns:
        for i in range(len(turn)):
            if (turn[i][0] == 'switch'):
                # Parse base info included in with the move
                parsedTurn = {'num':turnNum}
                turnNum = turnNum + 1
                parsedTurn['move'] = 'switch'
                parsedTurn['player'] = turn[i][1][0:3]
                parsedTurn['atkr'] = turn[i][1][5:] # everything after header 'pna: '
                parsedTurn['defender'] = turn[i][2]
                parsedTurn['atkrHP'] = -1
                parsedTurn['defenderHP'] = -1
                parsedTurn['selfStatus'] = ''
                parsedTurn['oppStatus'] = ''
                game['turns'].append(parsedTurn)
            elif (turn[i][0] == 'move'):
                # Parse base info included in with the move
                parsedTurn = {'num':turnNum}
                turnNum = turnNum + 1
                parsedTurn['move'] = turn[i][2]
                parsedTurn['player'] = turn[i][1][0:3]
                parsedTurn['atkr'] = turn[i][1][5:] # everything after header 'pna: '
                parsedTurn['defender'] = turn[i][3][5:]
                parsedTurn['defenderHP'] = -1
                parsedTurn['selfStatus'] = ''
                parsedTurn['oppStatus'] = ''
                parsedTurn['atkrHP'] = -1
                # Parse damage, status, heal, and faint info
                j = i + 1
                while ((j < len(turn)) and (turn[j][0] != 'switch') and (turn[j][0] != 'move')):
                    if (turn[j][0] == "-damage"): # check for damage (given as remaining hp)
                        match = match = re.match(ur'[0-9]+(?=\\)', turn[j][2])
                        if (match): # match any  number of digits followed by \
                            parsedTurn['defenderHP'] = int(match.group(0))
                    if (turn[j][0] == "-heal"): # check for healing (given as remaining hp)
                        match = match = re.match(ur'[0-9]+(?=\\)', turn[j][2])
                        if (match):
                            parsedTurn['atkrHP'] = int(match.group(0))
                    # determine whethers status self-inflicted
                    if (turn[j][0] == "-status"):
                        player = turn[j][1][0:4] # get player name
                        if player == parsedTurn['player']:
                            parsedTurn['selfStatus'] = turn[j][2]
                        else:
                            parsedTurn['oppStatus'] = turn[j][2]
                    if (turn[j][0] == "faint"):
                        parsedTurn['defenderHP'] = 0
                    j = j + 1
                game['turns'].append(parsedTurn)
            elif (turn[0] == 'win'):
                print turn[1][:-8]
                game['win'] = turn[1][:-8] # winner's name has '<script' at the end of it
                return game

parsedGame = parseLog("test-log.txt")
pp.pprint(parsedGame)
gameJSON = json.dumps(parsedGame)
    
