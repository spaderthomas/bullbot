# TODO: stabBonus(), typeBonus()
import Pokemon as p
class DamageCalculator():
	with open("pokemon.json", "r+b") as infile:
		pokemonDB = json.load(infile)

	with open("move-data.json", "r+b") as infile:
		moveDB = json.load(infile)

	with open("type-chart.json", "r+b") as infile:
		typeDB = json.load(infile)

	def calcAverageDamage(attacker, defender, move):
		if moveDB[move]['category'] == 'physical':
			attack = pokemonDB[attacker.name]['stats']['attack']
			defense = pokemonDB[defender.name]['stats']['defense']
			power = moveDB[move]['base power']
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		else:
			attack = pokemonDB[attacker.name]['stats']['special']
			defense = pokemonDB[defender.name]['stats']['special']
			power = moveDB[move]['base power']
			stab = stabModifier(move, attacker)
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		return damage

	def calcMaxDamage(attacker, defender, move):
		if moveDB[move]['category'] == 'physical':
			attack = pokemonDB[attacker.name]['stats']['attack']
			defense = pokemonDB[defender.name]['stats']['defense']
			power = moveDB[move]['base power']
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		else:
			attack = pokemonDB[attacker.name]['stats']['special']
			defense = pokemonDB[defender.name]['stats']['special']
			power = moveDB[move]['base power']
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		return damage

	def calcMinDamage(attacker, defender, move):
		if moveDB[move]['category'] == 'physical':
			attack = pokemonDB[attacker.name]['stats']['attack']
			defense = pokemonDB[defender.name]['stats']['defense']
			power = moveDB[move]['base power']
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		else:
			attack = pokemonDB[attacker.name]['stats']['special']
			defense = pokemonDB[defender.name]['stats']['special']
			power = moveDB[move]['base power']
			modifier = getModifier(attacker, defender, move)

			damage = math.floor(((0.84 * attacker * power / defense) + 2) * (modifier *  217 / 255))

		return damage

	def getModifier(attacker, defender, move):
		modifier = 1
		attackerTypes = pokemonDB[attacker]['types']
		defenderTypes = pokemonDB[attacker]['types']

		for attackerType in attackerTypes:
			if moveDB[move]['type'] == attackerType:
				modifier = modifier * 1.5
			for defenderType in defenderTypes:
				if defenderType in typeDB[attackerType]['immune']:
					modifier = 0
				elif defenderType in typeDB[attackerType]['s-e']:
					modifier = modifier * 2
				elif defenderType in typeDB[attackerType]['nve']:
					modifier = modifier * .5

if __name__ == "__main__":
	tauros = p.Pokemon("Tauros")
	egg = p.Pokemon("Exeggutor")

	print(calcAverageDamage(tauros, egg, "body-slam"))
