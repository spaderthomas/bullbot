import Pokemon, Player, DamageCalculator


class Battle():
	def __init__(self, first_player=None, second_player=None):
		if first_player is None and second_player is None:
			self.first_player = Player.Player()
			self.second_player = Player.Player()
			players = (self.first_player, self.second_player)
		self.turn = 0
		self.is_over = False

	def main(self):
		self.first_turn()
		while not self.turn > 10:
		        # self.get_player_moves()
			moves = [self.first_player.move(), self.second_player.move()]
			self.first_player.process_turn(moves[0], moves[1])
			self.second_player.process_turn(moves[1], moves[0])
			print("Pre sorted is: " + str(moves))
			# moves.sort(Player.player_move.comparator)
                        # print("Post sorted is: " + str(moves))

                        # self.turn += 1

        """ Generates each player's move for a turn. """
        def get_player_moves(self):
                first_player_move = self.first_player.move()
                second_player_move = self.second_player.move()
                self.first_player.process_turn(first_player_move, second_player_move)
                self.second_player.process_turn(second_player_move, first_player_move)

	""" Since the first turn is very homogenous but different from a generic turn,
		it gets its own method. Just has each player send out their first Pokemon,
		and update the game states."""
	def first_turn(self):
		first_player_move = self.first_player.make_first_move()
		second_player_move = self.second_player.make_first_move()
		self.first_player.process_turn(first_player_move, second_player_move)
		self.second_player.process_turn(second_player_move, first_player_move)
		self.turn += 1


b = Battle()
b.main()

print(b.first_player.current_out)
print(b.second_player.current_out)





