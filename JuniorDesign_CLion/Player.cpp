#include "Player.h"


Player::Player(std::string filepath,
	 std::unordered_map<std::string, Pokemon> team,
	 std::unordered_map<std::string, Pokemon> knownOpponentTeam,
	 std::unordered_map<std::string, Pokemon> opponentTeam)
{
	srand(time(0)); //TODO: there is almost definitely a better place for this
	this->team = team; //TODO: We're setting the team here and in buildTeam, is this necessary?
	this->knownOpponentTeam = knownOpponentTeam;
	this->opponentTeam = opponentTeam;
	this->hasWon = false;
	buildTeam(filepath);
}

Player::Player(string filepath)
{
	this->hasWon = false;
	buildTeam(filepath);
}

void Player::setCurrentOut(PlayerMove move)
{
	if (move.isSwitch)
		this->currentOut = move.pokemon.getName();
}

void Player::setOpponentCurrentOut(PlayerMove move)
{
	if (move.isSwitch)
		this->opponentCurrentOut = move.pokemon.getName();
}

/*	This method handles the logic of updating the opponent's team
 *  i.e. changing which Pokemon is currently out, which Pokemon are known,
 *  and what moves are known.
 
 *	After this method is called, everything except damage will be correctly updated
 *  in the player's model of the opponent's team.
 */
void Player::updateOpponentTeam(PlayerMove move)
{
	if (move.isSwitch)
	{
		if (this->knownOpponentTeam.find(tove.pokemon.getName()) == this->knownOpponentTeam().end())
		{
			Pokemon *p = new Pokemon(move.pokemon.getName());
			this->knownOpponentTeam[move.pokemon.getName()] = *p;
			setOpponentCurrentOut(move);
		} else
		{
			setOpponentCurrentOut(move);
		}
	} else 
	{
		Pokemon *p = new Pokemon(this->knownOpponentTeam(move.pokemon.getName()));
		if (!p->containsMove(move.moveName))
			pokemon.appenMove(move.moveName);
	}
}

PlayerMove Player::move()
{
	int moveType = rand() % 2;
	PlayerMove move;
	if (!moveType)
	{
		string switchPokemon = this->currentOut;
		while (switchPokemon == this->currentOut)
		{
			int randomChoice = rand() % this->team.size();
			auto it = this->team.begin();
			for (int i = 0; i < randomChoice; i++)
			{
				switchPokemon = it->first;
				it++;
			}
		}
		move.isSwitch = true;
		move.Pokemon = &this->team[switchPokemon];
		std::cout << "Switched to " << switchPokemon << std::endl;
	} else 
	{
		int i = rand() % 4;
		move.isSwitch = false;
		move.Pokemon = &this->team[this->currentOut];
		move.moveName = this->team[this->currentOut].getMove(i).moveName;
		std::cout << this->currentOut 
			<< " used " 
			<< this->team[this-currentOut].getMove(i).moveName
			<< std::endl;
	}
	return move;
}

void Player::buildTeam(std::string filepath)
{
	std::ifstream infile(filepath);
	std::string line;
	int i = 0;
	while (std::getline(infile, line))
	{
		//this python looks hairy, not really sure what "name" the elif conditionals refer to...
		//code for reference:
		
		/*if i % 6 == 0:
		 *	name = lines[i].strip("\n").replace(" ","-")
	     *	newPokemon = p.Pokemon(name)
		 *  self.team[name] = newPokemon
		 *elif not (i % 6 == 5):
		 *  self.team[name].addMove(lines[i].replace(" ","").lower().strip("\n"))
		 *elif i == 5:
		 *  self.set_current_out(player_move(True, newPokemon))
		 */
			
	}
}




