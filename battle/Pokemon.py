import json

class Pokemon():
	def __init__(self, name, typing=None, stats=None, moves=None, isBurned=False, isParalyzed=False, isAsleep=False, isFrozen=False):
		self.name = name
		self.isFrozen = isFrozen
		self.isAsleep = isAsleep
		self.isParalyzed = isParalyzed
		self.isBurned = isBurned
		if stats is None:
			self.stats = {}
		if typing is None:
			self.typing = []
		if moves is None:
			self.moves = []

		with open("../data/pokemon.json", "r") as infile:
			data = json.load(infile)
			for stat in data[self.name]["stats"]:
				self.stats[stat] = data[name]["stats"][stat]
				if stat == "hp":
					self.hp = data[name]["stats"][stat]
			for _type in data[name]["type"]:
				self.typing.append(_type)


	def __str__(self):
		pokemonString = "name: " + self.name + "\nhp: " + str(self.hp) + "\nmoves:"
		for move in self.moves:
			pokemonString += " " + move
		pokemonString += "\n--------"
		return pokemonString

	def addMove(self, move):
		self.moves.append(move)

if __name__ == "__main__":
	jarvis = Pokemon("Tauros")
	wayne = Pokemon("Exeggutor")
	print(jarvis.stats["hp"])
	print(wayne.stats)
	print(jarvis.typing)

