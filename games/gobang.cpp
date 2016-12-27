// Petter Strandmark 2013
// petter.strandmark@gmail.com
#include <iostream>
using namespace std;
#include <mcts.h>
#include "gobang.h"
void main_program()
{
	using namespace std;

	bool human_player = false;

	MCTS::ComputeOptions player0_options, player1_options, player2_options;
	player0_options.max_iterations = 1000000;
	player0_options.verbose = true;
	player1_options.max_iterations = 1000000;
	player1_options.verbose = true;
	player2_options.max_iterations = 1000000;
	player2_options.verbose = true;

	GoBangState state(6,6,6);
	while (state.has_moves()) {
		cout << endl << "State: " << state << endl;
	
		int is_to_move_player=(state.player_is_moved+1)%GoBangState::Support_Num_Players;

		GoBangState::Move move = GoBangState::no_move;
		if (is_to_move_player == 0) {
			move = MCTS::compute_move(state, player0_options);
			state.do_move(move);
		}
		else if(is_to_move_player == 1) {
			if (human_player) {
				MCTS::human_do_move(state);
				while (true) {
					cout << "Input your move: ";
					move = GoBangState::no_move;
					cin.clear();
					cin.sync();
					cin >> move;
					try {
						state.do_move(move);
						break;
					}
					catch (std::exception& ) {
						cout << "Invalid move." << endl;
					}
				}
			}
			else {
				move = MCTS::compute_move(state, player1_options);
				state.do_move(move);
			}
		}
		else if(is_to_move_player ==2){
			move=MCTS::compute_move(state,player2_options);
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
		main_program();
	}
	catch (std::runtime_error& error) {
		std::cerr << "ERROR: " << error.what() << std::endl;
		return 1;
	}
}
