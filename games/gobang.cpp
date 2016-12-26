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

	MCTS::ComputeOptions player1_options, player2_options;
	player1_options.max_iterations = 400000;
	player1_options.verbose = true;
	player2_options.max_iterations = 400000;
	player2_options.verbose = true;

	GoBangState state(10,10);
	while (state.has_moves()) {
		cout << endl << "State: " << state << endl;

		GoBangState::Move move = GoBangState::no_move;
		if (state.player_to_move == 1) {
			move = MCTS::compute_move(state, player1_options);
			state.do_move(move);
		}
		else {
			if (human_player) {
				while (true) {
					cout << "Input your move: ";
					move = GoBangState::no_move;
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
				move = MCTS::compute_move(state, player2_options);
				state.do_move(move);
			}
		}
	}

	cout << endl << "Final state: " << state << endl;

	if (state.get_result(2) == 1.0) {
		cout << "Player 1 wins!" << endl;
	}
	else if (state.get_result(1) == 1.0) {
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
