import re, json

def ExtractLetters(InputString):
    from string import ascii_letters
    return "".join([ch for ch in InputString if ch in (ascii_letters)])

def ExtractNumeric(InputString):
    from string import digits
    string = ""
    for ch in InputString:
    	if ch in digits:
    		string += ch
    	elif ch == ".":
    		string += ch
    return string

with open('moveFreqTextFile', 'r') as file:
	movesetData = {}
	pokemon = ""
	count = 0
	lines = file.readlines()
	for i in range(0, len(lines)):
		print lines[i]
		if count == 2:
			pokemon = ExtractLetters(lines[i])
		if lines[i] == " | Moves                                  |\n":
			tempMoveDict = {}
			while lines[i+1] != " +----------------------------------------+\n":
				i = i + 1
				move = ExtractLetters(lines[i]).lower()
				percent = ExtractNumeric(lines[i])
				print percent
				tempMoveDict[move] = float(percent)

			movesetData[pokemon] = tempMoveDict
		if lines[i] == " +----------------------------------------+\n":
			count += 1
		else:
			count = 0

with open('moveFrequency.json', 'r+b') as outfile:
	json.dump(movesetData, outfile, indent=4, sort_keys=True)
print movesetData

