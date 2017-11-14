import time
import io
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

class Selenium():
	BASE_URL = "http://play.pokemonshowdown.com/"

	def __init__(self, url=BASE_URL):
		self.url = url
		self.driver = webdriver.Chrome('/users/tommy2/Documents/TaurosBot/chromedriver')

	def start(self):
		self.driver.get(self.url)
		self.login()
		self.importTeam("RBY")
		self.acceptChallenge()

	# waits looking for an element to appear
	# useful instead of using lots of sleep commands to compensate for loading time
	def waitLookingForTagByName(self, tag):
		while True:
			try:
				elem = self.driver.find_element_by_name(tag)
				return elem
			except:
				time.sleep(1)


	def login(self):
		chooseNameField = self.waitLookingForTagByName('login') # Find the choose name button
		chooseNameField.click()
		time.sleep(1)
		userField = self.driver.find_element_by_name('username')
		userField.click()
		userField.send_keys('TaurosBotV1', Keys.RETURN)
		pwdField = self.waitLookingForTagByName('password')
		pwdField.send_keys('bullcrit', Keys.RETURN)

	def acceptChallenge(self):
		challButton = self.waitLookingForTagByName("acceptChallenge")
		challButton.click();

	# can only take one team at a time
	# could have multiple teams by specifying multiple filenames
	def importTeam(self, filename):
		# open the file that contains the import
		teamFile = open(str(filename) + ".txt", "r")

		# open the teambuilder
		teambuilderButton = self.driver.find_element_by_xpath("//button[@value='teambuilder']")
		teambuilderButton.click()

		newButton = self.waitLookingForTagByName("backup")
		newButton.click()

		importBox = self.driver.find_element_by_xpath("//div[@class='teamedit']/textarea[@class='textbox']")
		for line in teamFile:
			importBox.send_keys(line)

		saveButton = self.driver.find_element_by_name("saveBackup")
		saveButton.click()

		homeButton = self.driver.find_element_by_xpath("//i[@class='fa fa-home']")
		homeButton.click()

	def selectTeam(self):
		teamSelectPanelButton = self.driver.find_element_by_xpath("//button[@value='class teamselect']")
		teamSelectPanelButton.click()
		teamSelectButton = self.waitLookingForTagByName("selectTeam")
		teamSelectButton.click()

	def chooseMove(self, moveNumber):
		moveButton = self.driver.find_element_by_xpath("//button[@name='chooseMove'][" + move + "]")
		moveButton.click()

	def switch(self, value):
		switchButton = self.driver.find_element_by_xpath("//button[@name='chooseSwitch'][" + value + "]")
		switchButton.click()

	def battle():


	##
	# TO DO
	# selectTeam
	# chooseAttack()
	# switchPokemon()
	##



x = Selenium()
x.start()
time.sleep(10)
x.chooseMove("1")
