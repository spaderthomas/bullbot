# bull-bot

To build this project, create a bin directory and run cmake pointed to your source directory.

And don't forget to eat a delicious, sandy pear today.

# Developer notes

I decided we needed a standardized way to represent a Pokemon battle. Included in the /python/log-scraping folder is a web scraper I wrote to gather logs from Pokemon Showdown's replay repository, as well as a parser to put them into a data structure. I chose JSON to do this because JSON is good. Here's a rundown of what it looks like


```javascript
var battle =
    {
        "players" : {
            "p1a": "player_name",
            "p2a": "player_name"
        },
        "win": "player_name",
        "turns": [
            {
             "atkr": "pokemon executor of move. either pokemon switched to or the attacker",
             "defender": "opposing pokemon on the field when move is executed"
             "defenderHP": "-1 for no change, otherwise the remaining HP of the defender",
             "atkrHP": "-1 for no change, otherwise the remaining HP of the attacker",
             "selfStatus": "status inflicted on attacker as result of turn",
             "oppStatus": "status inflicted on defender as resulf of turn",
             "move": "name of move used by attacker, switch if move was a switch",
             "player": "name of player who the move, in {p1a, p2a}",
             "num": "turn number"
            }
        ]
    }
```             
             
