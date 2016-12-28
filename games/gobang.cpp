// Petter Strandmark 2013
// petter.strandmark@gmail.com
#include <iostream>
using namespace std;
#include <mcts.h>
#include "gobang.h"

template<int num_player, int num_iterations>
void main_program(const int human_player_id=-1)
{
	using namespace std;

	bool human_player = false;

	if(human_player_id>=0&&human_player_id<num_player)
		human_player=true;

	MCTS::ComputeOptions player_options[num_player];
	
	for(int i=0;i<num_player;i++)
	{
		player_options[i].max_iterations=num_iterations;
		player_options[i].verbose = true;
	}
	//player0_options.max_iterations =;
	//player0_options.verbose = true;
	//player1_options.max_iterations = 400000;
	//player1_options.verbose = true;
	//player2_options.max_iterations = 400000;
	//player2_options.verbose = true;

	GoBangState<num_player> state(6,6,6);
	while (state.has_moves()) 
	{
		cout << endl << "State: " << state << endl;
	
		int is_to_move_player=(state.player_is_moved+1)%GoBangState<num_player>::Support_Num_Players;

		typename GoBangState<num_player>::Move move = GoBangState<num_player>::no_move;

		if(human_player&&human_player_id==is_to_move_player)
		{
			MCTS::human_do_move(state);
			while (true) 
			{
				cout << "Input your move: ";
				move = GoBangState<num_player>::no_move;
				cin.clear();
				cin.sync();
				cin >> move;
				try 
				{
					state.do_move(move);
					break;
				}
				catch (std::exception& ) 
				{
					cout << "Invalid move." << endl;
				}
			}
		}
		else
		{
			move = MCTS :: compute_move(state, player_options[is_to_move_player]);
			state.do_move(move);	
		}
	}

	cout << endl << "Final state: " << state << endl;

	//auto getPlayerForResult=[](int id){return (id+1)%GoBangState::Support_Num_Players;};

	if (state.get_result(0) == 1.0) {
		cout << "Player 0 wins!" << endl;
	}
	else if (state.get_result(1) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else if (state.get_result(2) == 1.0) {
		cout << "Player 2 wins!" << endl;
	}
	else {
		cout << "Nobody wins!" << endl;
	}
}

int main()
{
	try {
		main_program<3,4000000>();
	}
	catch (std::runtime_error& error) {
		std::cerr << "ERROR: " << error.what() << std::endl;
		return 1;
	}
}
