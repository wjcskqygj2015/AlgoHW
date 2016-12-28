// Petter Strandmark 2013
// petter.strandmark@gmail.com
#include <iostream>
using namespace std;
#include "gobang.h"
#include <mcts.h>

template<int num_players, int num_iterations>
void
main_program(const int human_player_id = -1)
{
  using namespace std;

  bool human_player = false;

  if (human_player_id >= 0 && human_player_id < num_players)
    human_player = true;
  // MCTS::ComputeOptions player_options[num_players], player1_options,
  // player2_options;
  MCTS::ComputeOptions player_options[num_players];
  for (int i = 0; i < num_players; i++)
  {
    player_options[i].max_iterations = num_iterations;
    player_options[i].verbose = true;
  }
  // player0_options.max_iterations = 400000;
  // player0_options.verbose = true;
  // player1_options.max_iterations = 400000;
  // player1_options.verbose = true;
  // player2_options.max_iterations = 400000;
  // player2_options.verbose = true;

  GoBangState<num_players> state(6, 6, 6);
  while (state.has_moves())
  {
    cout << endl << "State: " << state << endl;

    int is_to_move_player = (state.player_is_moved + 1) %
                            GoBangState<num_players>::Support_Num_Players;

    typename GoBangState<num_players>::Move move =
      GoBangState<num_players>::no_move;

    if ((human_player == true) && (is_to_move_player == human_player_id))
    {
      MCTS::human_do_move(state);
      while (true)
      {
        cout << "Input your move: ";
        move = GoBangState<num_players>::no_move;
        cin.clear();
        cin.sync();
        cin >> move;
        try
        {
          state.do_move(move);
          break;
        }
        catch (std::exception&)
        {
          cout << "Invalid move." << endl;
        }
      }
    }
    else
    {
      move = MCTS::compute_move(state, player_options[is_to_move_player]);
      state.do_move(move);
    }
  }

  cout << endl << "Final state: " << state << endl;

  // auto getPlayerForResult=[](int id){return
  // (id+1)%GoBangState::Support_Num_Players;};
  for (int i = 0; i < num_players; i++)
    if (state.get_result(i) == 1.0)
    {
      cout << "Player " << i << " wins!" << endl;
      return;
    }

  cout << "Nobody wins!" << endl;
}

int
main()
{
  try
  {
    main_program<3,1000000>();
  }
  catch (std::runtime_error& error)
  {
    std::cerr << "ERROR: " << error.what() << std::endl;
    return 1;
  }
}
