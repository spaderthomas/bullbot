import Pokemon, Player

class Battle():
	""" This class handles the logic of battling. That is, it calculates what
	each player needs to know happened on a turn, routes this information between
	players, and keeps track if the battle is over.

	Flow of a battle goes as such:
	1. Both players send a player_move to the Battle object
	2. The battle object determines the order of the moves
	3. Moves are executed in order (this means that an attack
	   is not executed if the attacker faints)
	4. Battle sends a battle_move to both players
	5. Player objects then handle updating states

	"""
	def __init__(self, first_player=None, second_player=None):
		if first_player is None:
			self.first_player = Player.Player("sample-team.txt")
		if second_player is None:
			self.second_player = Player.Player("sample-team.txt")
		turn = 0
		isOver = False

	def calcDamage(attacker, defender):
		return

	def main(self):
		while not first_player.has_won and not second_player.has_won
			p1_move = self.first_player.move()
			p2_move = self.second_player.move()
			self.first_player.process_turn(p1_move, p2_move)
			self.second_player.process_turn(p2_move, p1_move)


b = Battle()
b.main()

print(b.first_player.current_out)
print(b.second_player.current_out)





