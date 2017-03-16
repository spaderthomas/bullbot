import json

with open("move-data-temp.json", "r") as infile:
	data = json.load(infile)

new_data = {}
for move in data:
	name_ = move['name']
	name = move['name'].lower().replace(" ", "")
	new_data[name] = {}
	new_data[name]['accuracy'] = move['accuracy']
	new_data[name]['category'] = move['category'].lower()
	new_data[name]['power'] = move['power']
	new_data[name]['pp'] = move['pp']
	new_data[name]['type'] = move['type'].lower()

with open("move-data.json", "w") as outfile:
	json.dump(new_data, outfile, sort_keys=True, indent=4)