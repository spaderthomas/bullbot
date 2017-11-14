class temp():
	def __init__(self):
		self.moves = {}
		for i in range(10):
			self.moves[i] = i

test = temp()
print(test.moves)
test_2 = temp()
test_2.moves["new"] = "new"
print(test_2.moves)
print(test.moves)

