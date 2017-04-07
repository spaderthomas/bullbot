#include "Battle.h"
#include "BattleLogic.h"
#include <iostream>
int main() {
	Battle battle;
	Party a;
	a.name = "Ash";

	Pokemon pikachu;
	pikachu.name = "Pikachu";
	pikachu.attack = 10;
	pikachu.defense = 10;
	Move ptackle;
	ptackle.name = "tackle";
	pikachu.moveset.push_back(ptackle);
	Move irontail;
	irontail.name = "iron tail";
	pikachu.moveset.push_back(irontail);

	Pokemon bulbasaur;
	bulbasaur.name = "Bulbasaur";
	bulbasaur.attack = 10;
	bulbasaur.defense = 10;
	a.pokemon.push_back(pikachu);
	a.pokemon.push_back(bulbasaur);
	Party b;
	b.name = "Gary";
	Pokemon squirtle;
	squirtle.name = "Squirtle";
	Move water_gun;
	water_gun.power = 20;
	water_gun.name = "Water Gun";
	squirtle.moveset.push_back(water_gun);
	b.pokemon.push_back(squirtle);

	a.active_pokemon = &a.pokemon[0];//pokemon on field
	b.active_pokemon = &b.pokemon[0];

	battle.parties.add(a);
	battle.parties.add(b);

	std::printf("party  pers name %s\n", battle.parties.head->data.name.c_str());
	std::printf("%d\n", attackDamage(bulbasaur, water_gun, pikachu));
	while (1) {
		std::cin.get();
		battle.conduct_turn();
		//battle.make_move();
		//battle.end_turn();
	}
	return 0;
}