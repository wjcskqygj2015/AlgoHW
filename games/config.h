#ifndef CONFIG_H_
#define CONFIG_H_
#include"../mcts.h"
namespace CONFIG
{
	constexpr const int max_iterations=1000000;
	constexpr const int num_players=3;
	constexpr const int chessboard_size=6;
	
	constexpr const int human_player_id=-1;

	//const MCTS::ComputeOptions player_options[num_players]={MCTS::active_search, MCTS::sag_search};	
	const MCTS::ComputeOptions player_options[num_players]={};	
	//static ComputeOptions default_search(1000000,false,1.0,1.0,2.0);
	//static ComputeOptions active_search(1000000,false,1.0,1.0,4.0);
	//static ComputeOptions sag_search(1000000,false,1.0,1.0,1.0);
	//static ComputeOptions suppress_search(1000000,false,1.0,4.0,2.0);
	//static ComputeOptions selfwin_search(1000000,false,1.0,0.5,2.0);
}
#endif
