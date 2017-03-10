import json

with open("data.json", "r") as infile:
	move_data = json.load(infile)
	myDict = move_data['injectRpcs'][1][1]['moves']
	for thing in myDict:
		del thing['genfamily']
		print(thing)

with open("move-data-temp.json", "w") as outfile:
	json.dump(myDict, outfile, sort_keys=True, indent=4)


