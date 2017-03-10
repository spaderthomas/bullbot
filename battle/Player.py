import Pokemon as p
import json, random

class Player():
	def __init__(self, filepath, team=None, knownOpponentTeam=None, opponentTeam=None):
		if team is None:
			self.team = {}
		if knownOpponentTeam is None:
			self.knownOpponentTeam = {}
		if opponentTeam is None:
			self.opponentTeam = {}
		self.has_won = False;

		self.build_team(filepath)

	def set_current_out(self, move):
		if move.isSwitch:
			self.current_out = move.pokemon.name

	def set_opponent_current_out(self, move):
		if move.isSwitch:
			self.opponent_current_out = move.pokemon.name

	""" This method handles the logic of updating the opponent's team
	i.e. changing which Pokemon is currently out, which Pokemon are known,
	and what moves are known.

	After this method is called, everything except damage will be correctly updated
	in the player's model of the opponent's team. """
	def update_opponent_team(self, move):
		if move.isSwitch:
			if move.pokemon.name not in self.knownOpponentTeam:
				newPokemon = p.Pokemon(move.pokemon.name)
				self.knownOpponentTeam[move.pokemon.name] = newPokemon
				self.set_opponent_current_out(move)
			else:
				self.set_opponent_current_out(move)
		else:
			pokemon = self.knownOpponentTeam[move.pokemon.name]
			if move.moveName not in pokemon.moves:
				pokemon.moves.append(move.moveName)

	def process_turn(self, your_move, opponent_move):
		self.update_opponent_team(opponent_move)
		self.set_current_out(your_move)


	def move(self):
		move_type = random.randint(0,1)
		if move_type is 0:
			switch = self.current_out
			while (switch == self.current_out):
				switch = random.choice(list(self.team.keys()))
			move = player_move(True, self.team[switch])
			print("Switched to " + switch)
		else:
			i = random.randint(0,3)
			move = player_move(False, self.team[self.current_out], self.team[self.current_out].moves[i])
			print(self.current_out + " used " + self.team[self.current_out].moves[i])

		return move

	"""
	The format of each individual pokemon is this:
	Jynx
	Blizzard
	Psychic
	Lovely Kiss
	Rest
	newline
	...
	"""
	def build_team(self, filepath):
		with open(filepath, "r") as infile:
			lines = infile.readlines()

		for i in range(len(lines)):
			if i % 6 == 0:
				name = lines[i].strip("\n").replace(" ","-")
				newPokemon = p.Pokemon(name)
				self.team[name] = newPokemon
			elif not (i % 6 == 5):
				self.team[name].addMove(lines[i].replace(" ","").lower().strip("\n"))
			elif i == 5:
				self.set_current_out(player_move(True, newPokemon))


	def __str__(self):
		print("i exist!!")

class player_move():
	def __init__(self, isSwitch, pokemon, moveName=None):
		self.isSwitch = isSwitch
		self.pokemon = pokemon
		self.moveName = moveName if moveName is not None else ""

if __name__ == "__main__":
	thomas = Player("sample-team.txt")
	for i in range(10):
		move = thomas.move()
		thomas.process_turn(move, player_move(True, p.Pokemon("Tauros")))

