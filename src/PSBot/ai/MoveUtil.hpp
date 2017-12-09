void getAvailableActions(std::vector<PokemonData>& team, action_arr_t& availableActions) {
  fox_for(indxPkmn, team.size()) {
    PokemonData& pokemon = team[indxPkmn];
    if (pokemon.active) {
      // Push back all moves
      availableActions.push_back(0);
      availableActions.push_back(1);
      availableActions.push_back(2);
      availableActions.push_back(3);
    } else {
      if (!pokemon.fainted) {
        // Push back all switches. Switching to team[0] would be represented by 4
        availableActions.push_back(indxPkmn + 4); 
      }
    }
  }
}
