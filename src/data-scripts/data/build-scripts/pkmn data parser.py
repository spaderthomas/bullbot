import requests, bs4, io, re, json, time


def getTypes(dexNumber):
	urlStr = ""

	urlStr = "http://www.serebii.net/pokedex/" + format(dexNumber, '03') + ".shtml"
	pokemonFile = requests.get(urlStr)
	soup = bs4.BeautifulSoup(pokemonFile.text, 'html.parser')
	htmlString = ""
	typeTags = ""
	finalTypeArray = []

	# finds all a tags inside td tags, converts tag to string, adds to htmlString
	for tag in soup.select('td a'):
		htmlString += (str(tag)) #+ "\n----------------\n"

	# matches against aforementioned tags with regex for type file, adds then to typeTags
	for i in re.findall(r'/pokedex/[a-z]*\.shtml', htmlString):
		typeTags += i

	# looks in typeTags for things that match type names, returns
	# bug|dragon|electric|fighting|fire|flying|ghost|grass|ground|ice|normal|psychic|poison|rock|water
	for i in re.findall(r'[a-z]*(?=\.shtml)', typeTags):
		if i != "":
			finalTypeArray.append(i)

	return finalTypeArray

def getMoves(dexNumber):
	urlStr = ""

	urlStr = "http://www.serebii.net/pokedex/" + format(dexNumber, '03') + ".shtml"
	pokemonFile = requests.get(urlStr)
	soup = bs4.BeautifulSoup(pokemonFile.text, 'html.parser')
	htmlString = ""
	moveLines = ""
	finalMovesArray = []

# finds all a tags inside td tags, converts tag to string, adds to htmlString
	for tag in soup.select('td a'):
		htmlString += (str(tag)) #+ "\n----------------\n"

	# matches against aforementioned tags with regex for type file, adds then to typeTags
	for i in re.findall(r'/attackdex-rby/[a-z]*\.shtml', htmlString):
		moveLines += i

	for i in re.findall(r'[a-z]*(?=\.shtml)', moveLines):
		if i != "" and i not in finalMovesArray:
			finalMovesArray.append(i)

	return finalMovesArray

def getStats(dexNumber):
	urlStr = ""

	urlStr = "http://www.serebii.net/pokedex/" + format(dexNumber, '03') + ".shtml"
	pokemonFile = requests.get(urlStr)
	soup = bs4.BeautifulSoup(pokemonFile.text, 'html.parser')
	htmlString = ""
	statArray = []

# finds all tr tags inside td tags, converts tag to string, adds to htmlString
	for tag in soup.select('tr td'):
		htmlString += (str(tag)) + "\n----------------\n"

	for i in re.findall(r'(?<=<td align="center" class="fooinfo">)[0-9]{1,3}(?=</td>)', htmlString):
		statArray.append(i)

	return statArray

def getName(dexNumber):
	urlStr = ""

	urlStr = "http://www.serebii.net/pokedex/" + format(dexNumber, '03') + ".shtml"
	pokemonFile = requests.get(urlStr)
	soup = bs4.BeautifulSoup(pokemonFile.text, 'html.parser')
	htmlString = ""
	name = ""

	for tag in soup.select('head title'):
		htmlString += (str(tag))

	for i in re.findall(r'(?<=<title>Serebii.net Pok.{2}dex - #[0-9]{3} - )[a-zA-Z]*', htmlString):
		name = i

	return name


def getPokemonInfo(dexNumber):
	name = getName(dexNumber)
	typeArray = getTypes(dexNumber)
	moveArray = getMoves(dexNumber)
	statArray = getStats(dexNumber)

	return name + "\n" + ", ".join(typeArray) + "\n" + ", ".join(moveArray) + "\n" + ", ".join(statArray) + "\n-----\n"

def toJSON(x):
	with open("pokemon.json", "r+b") as infile:
		jsonDict = json.load(infile)
	for i in range(x,x+1):
		name = getName(i)
		jsonDict[name] = {}
		jsonDict[name]['type'] = getTypes(i)
		jsonDict[name]['moves'] = getMoves(i)
		jsonDict[name]['stats'] = {}
		statArray = getStats(i)

		jsonDict[name]['stats']['hp'] = statArray[0]
		jsonDict[name]['stats']['attack'] = statArray[1]
		jsonDict[name]['stats']['defense'] = statArray[2]
		jsonDict[name]['stats']['special'] = statArray[3]
		jsonDict[name]['stats']['speed'] = statArray[4]

		with open("pokemon.json", "r+b") as json_file:
			json.dump(jsonDict, json_file, indent=4, sort_keys=True)

	print(jsonDict)

for i in range(143,152):
	toJSON(i)
	time.sleep(10)