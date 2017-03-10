# TODO: stabBonus(), typeBonus()
import Pokemon as p
import json, math, pdb

with open("pokemon.json", "r") as infile:
	pokemonDB = json.load(infile)

with open("move-data.json", "r") as infile:
	moveDB = json.load(infile)

with open("type-chart.json", "r") as infile:
	typeDB = json.load(infile)

damageType = {'min' : 217, 'avg': 236, 'max': 255}

""" Accepts a Pokemon object for attacker and defender, and a string
representing the move used by attacker. Returns amount of damage
dealt."""
def calcAverageDamage(attacker, defender, move):
	return calcDamage(attacker, defender, move, "avg")

def calcMaxDamage(attacker, defender, move):
	return calcDamage(attacker, defender, move, "max")

def calcMinDamage(attacker, defender, move):
	return calcDamage(attacker, defender, move, "min")

def calcDamage(attacker, defender, move, level):
	if moveDB[move]['category'] == 'physical':
		# pdb.set_trace()
		attack = pokemonDB[attacker.name]['stats']['attack']
		defense = pokemonDB[defender.name]['stats']['defense']
		power = moveDB[move]['power']
		mod = getModifier(attacker, defender, move)
		return math.floor(((0.84 * (attack / defense) * power) + 2) * (mod *  damageType[level] / 255))
	elif moveDB[move]['category'] == 'special':
		special_attack = pokemonDB[attacker.name]['stats']['special']
		special = pokemonDB[defender.name]['stats']['special']
		power = moveDB[move]['power']
		mod = getModifier(attacker, defender, move)
		return math.floor(((0.84 * (special_attack / special) * power) + 2) * (mod *  damageType[level] / 255))
	else:
		return 42


def getModifier(attacker, defender, move):
	modifier = 1
	attackerTypes = pokemonDB[attacker.name]['type']
	defenderTypes = pokemonDB[defender.name]['type']
	moveType = moveDB[move]['type']

	for attackerType in attackerTypes:
		if moveDB[move]['type'] == attackerType:
			modifier = modifier * 1.5
	for defenderType in defenderTypes:
		if defenderType in typeDB[moveType]['immune']:
			modifier = 0
		elif defenderType in typeDB[moveType]['s-e']:
			modifier = modifier * 2
		elif defenderType in typeDB[moveType]['nve']:
			modifier = modifier * .5


	return modifier

if __name__ == "__main__":
	tauros = p.Pokemon("Tauros")
	egg = p.Pokemon("Exeggutor")
	print(calcMinDamage(tauros, egg, "bodyslam"))
	print(calcAverageDamage(tauros, egg, "bodyslam"))
	print(calcMaxDamage(tauros, egg, "bodyslam"))
