// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include "../mcts.h"


namespace CONST_STATIC_ARRAY_GENERATOR
{
	template<char ... args> struct ArrayHolder { static const char data[sizeof ... (args)]; };

	template<char ... args>
	const char ArrayHolder<args ...>::data[sizeof ... (args)] ={ args ... };

	template<size_t N, template<size_t> class F, char ... args>
	struct generate_array_impl { typedef typename generate_array_impl<N-1,F,F<N>::value,args...>::result result; };

	template<template<size_t> class F, char ... args>
	struct generate_array_impl<0, F, args ...> { typedef ArrayHolder<F<0>::value, args...> result; };

	template<size_t N, template<size_t> class F>
	struct generate_array { typedef typename generate_array_impl<N-1,F>::result result; };

	constexpr char player_sign_map[]={'X','O','L','G','H','T','S','V','M'}; 

	template<size_t index>
	struct SignGenerator { static const char value=player_sign_map[index]; };

}


template<int num_players>
class GoBangState
{
	typedef typename CONST_STATIC_ARRAY_GENERATOR::generate_array<num_players,CONST_STATIC_ARRAY_GENERATOR::SignGenerator>::result markers_result;
public:
  // static const int DIM=2;
  typedef struct Move
  {
    short int x;
    short int y;
    short int z;
    friend std::istream& operator>>(std::istream& in, struct Move& move)
    {
      in >> move.x >> move.y >> move.z;
      return in;
    }
    friend std::ostream& operator<<(std::ostream& out, const struct Move& move)
    {
      out << move.x << ' ' << move.y << ' ' << move.z;
      return out;
    }
    bool operator==(const struct Move& ot) const
    {
      return (this->x == ot.x) && (this->y == ot.y) && (this->z == ot.z);
    }
    bool operator!=(const struct Move& ot) const { return !((*this) == ot); }
    bool operator<(const struct Move& ot) const
    {
      return (this->x < ot.x) || ((this->x == ot.x) && (this->y < ot.y)) ||
             ((this->x == ot.x) && (this->y == ot.y) && (this->z < ot.z));
    }
  } Move;
  static constexpr const Move no_move={-1,-1,-1};

  static constexpr const int Support_Num_Players = num_players;

  static constexpr const char (&player_markers)[Support_Num_Players]= markers_result::data;

	static constexpr const char none_player_marker = '.';

  GoBangState(short int num_xs_ = 6, short int num_ys_ = 7,
              short int num_zs_ = 6)
    : player_is_moved(-1+Support_Num_Players), 
			num_xs(num_xs_), num_ys(num_ys_), num_zs(num_zs_), 
			last_x(-1), last_y(-1), last_z(-1)
  {

    board.resize(num_xs, vector<vector<char>>(
                           num_ys, vector<char>(num_zs, none_player_marker)));

    for (short int x = 0; x < num_xs; ++x)
      for (short int y = 0; y < num_ys; ++y)
        for (short int z = 0; z < num_zs; ++z)
          reserved_moves.push_back({x, y, z});
  }

  void do_move(Move move)
  {
    attest(0 <= move.x && move.x < num_xs);
    attest(0 <= move.y && move.y < num_ys);
    attest(0 <= move.z && move.z < num_zs);
    attest(board[move.x][move.y][move.z] == none_player_marker);
    check_invariant();

    player_is_moved ++;
    player_is_moved %= Support_Num_Players;
    // int x = num_xs - 1;
    // while (board[x][move] != none_player_marker) x--;
    board[move.x][move.y][move.z] = player_markers[player_is_moved];
    last_x = move.x;
    last_y = move.y;
    last_z = move.z;

    for (auto it = reserved_moves.cbegin(); it != reserved_moves.cend(); it++)
      if (*it == Move({move.x, move.y, move.z}))
      {
        reserved_moves.erase(it);
        break;
      }

  }

  template<typename RandomEngine>
  void do_random_move(RandomEngine* engine)
  {
    dattest(has_moves());
    check_invariant();
    static std::uniform_int_distribution<short int> moves_x(0, num_xs - 1);
    static std::uniform_int_distribution<short int> moves_y(0, num_ys - 1);
    static std::uniform_int_distribution<short int> moves_z(0, num_zs - 1);

    while (true)
    {
      Move move = {moves_x(*engine), moves_y(*engine), moves_z(*engine)};
      if (board[move.x][move.y][move.z] == none_player_marker)
      {
        do_move(move);
        return;
      }
    }
  }

  bool has_moves() const
  {
    check_invariant();

    char winner = get_winner();
    if (winner != none_player_marker)
    {
      return false;
    }

    if (!reserved_moves.empty())
      return true;
    return false;
  }

  std::vector<Move> get_moves() const
  {
    check_invariant();

    char winner = get_winner();
    if (winner != none_player_marker)
      return std::vector<Move>();
    return reserved_moves;
  }

	bool has_winner() const
	{
		return get_winner() != none_player_marker;
	}

  char get_winner() const
  {

    static const int JOIN_NUM = 5;

    int up = 0, down = 0;
    int left = 0, right = 0;
    int front = 0, back = 0;
    int increase = 0, decrease = 0;
    
		if (last_y < 0)
    {
      return none_player_marker;
    }

    // We only need to check around the last piece played.
    auto piece = board[last_x][last_y][last_z];

    // along x
    up = 0, down = 0;
    for (short int x = last_x - 1; x >= 0 && board[x][last_y][last_z] == piece;
         --x)
      up++;
    for (short int x = last_x + 1;
         x < num_xs && board[x][last_y][last_z] == piece; ++x)
      down++;
    if (up + 1 + down >= JOIN_NUM)
      return piece;

    // along y
    left = 0, right = 0;
    for (short int y = last_y - 1; y >= 0 && board[last_x][y][last_z] == piece;
         --y)
      left++;
    for (short int y = last_y + 1;
         y < num_ys && board[last_x][y][last_z] == piece; ++y)
      right++;
    if (left + 1 + right >= JOIN_NUM)
      return piece;

    // along z
    front = 0, back = 0;
    for (short int z = last_z - 1; z >= 0 && board[last_x][last_y][z] == piece;
         --z)
      front++;
    for (short int z = last_z + 1;
         z < num_zs && board[last_x][last_y][z] == piece; ++z)
      back++;
    if (front + 1 + back >= JOIN_NUM)
      return piece;

    // along xy (0,0) - (x+,y+)
    up = 0, down = 0;
    for (short int x = last_x - 1, y = last_y - 1;
         x >= 0 && y >= 0 && board[x][y][last_z] == piece; --x, --y)
      up++;
    for (short int x = last_x + 1, y = last_y + 1;
         x < num_xs && y < num_ys && board[x][y][last_z] == piece; ++x, ++y)
      down++;
    if (up + 1 + down >= JOIN_NUM)
      return piece;

    // along xy (0,0) - (x-,y+)
    up = 0, down = 0;
    for (short int x = last_x + 1, y = last_y - 1;
         x < num_xs && y >= 0 && board[x][y][last_z] == piece; ++x, --y)
      up++;
    for (short int x = last_x - 1, y = last_y + 1;
         x >= 0 && y < num_ys && board[x][y][last_z] == piece; --x, ++y)
      down++;
    if (up + 1 + down >= JOIN_NUM)
      return piece;

    // along yz (0,0) - (y+,z+)
    left = 0, right = 0;
    for (short int y = last_y - 1, z = last_z - 1;
         y >= 0 && z >= 0 && board[last_x][y][z] == piece; --y, --z)
      left++;
    for (short int y = last_y + 1, z = last_z + 1;
         y < num_ys && z < num_zs && board[last_x][y][z] == piece; ++y, ++z)
      right++;
    if (left + 1 + right >= JOIN_NUM)
      return piece;

    // along yz (0,0) - (y+,z-)
    left = 0, right = 0;
    for (short int y = last_y + 1, z = last_z - 1;
         y < num_ys && z >= 0 && board[last_x][y][z] == piece; ++y, --z)
      left++;
    for (short int y = last_y - 1, z = last_z + 1;
         y >= 0 && z < num_zs && board[last_x][y][z] == piece; --y, ++z)
      right++;
    if (left + 1 + right >= JOIN_NUM)
      return piece;

    // along zx (0,0) - (z+,x+)
    front = 0, back = 0;
    for (short int z = last_z - 1, x = last_x - 1;
         z >= 0 && x >= 0 && board[x][last_y][z] == piece; --z, --x)
      front++;
    for (short int z = last_z + 1, x = last_x + 1;
         z < num_zs && x < num_xs && board[x][last_y][z] == piece; ++z, ++x)
      back++;
    if (front + 1 + back >= JOIN_NUM)
      return piece;

    // along zx (0,0) - (z+,x-)
    front = 0, back = 0;
    for (short int z = last_z + 1, x = last_x - 1;
         z < num_zs && x >= 0 && board[x][last_y][z] == piece; ++z, --x)
      front++;
    for (short int z = last_z - 1, x = last_x + 1;
         z >= 0 && x < num_xs && board[x][last_y][z] == piece; --z, ++x)
      back++;
    if (front + 1 + back >= JOIN_NUM)
      return piece;

    // along xyz (0,0,0) - (x+,y+,z+)
    increase = 0, decrease = 0;
    for (short int x = last_x - 1, y = last_y - 1, z = last_z - 1;
         x >= 0 && y >= 0 && z >= 0 && board[x][y][z] == piece; --x, --y, --z)
      decrease++;
    for (short int x = last_x + 1, y = last_y + 1, z = last_z + 1;
         x < num_xs && y < num_ys && z < num_zs && board[x][y][z] == piece;
         ++x, ++y, ++z)
      increase++;
    if (increase + 1 + decrease >= JOIN_NUM)
      return piece;

    // along xyz (0,0,0) - (x+,y+,z-)
    increase = 0, decrease = 0;
    for (short int x = last_x - 1, y = last_y - 1, z = last_z + 1;
         x >= 0 && y >= 0 && z < num_zs && board[x][y][z] == piece;
         --x, --y, ++z)
      decrease++;
    for (short int x = last_x + 1, y = last_y + 1, z = last_z - 1;
         x < num_xs && y < num_ys && z >= 0 && board[x][y][z] == piece;
         ++x, ++y, --z)
      increase++;
    if (increase + 1 + decrease >= JOIN_NUM)
      return piece;

    // along xyz (0,0,0) - (x+,y-,z+)
   	increase = 0, decrease = 0;
    for (short int x = last_x - 1, y = last_y + 1, z = last_z - 1;
         x >= 0 && y < num_ys && z >= 0 && board[x][y][z] == piece;
         --x, ++y, --z)
      decrease++;
    for (short int x = last_x + 1, y = last_y - 1, z = last_z + 1;
         x < num_xs && y >= 0 && z < num_zs && board[x][y][z] == piece;
         ++x, --y, ++z)
      increase++;
    if (increase + 1 + decrease >= JOIN_NUM)
      return piece;

    // along xyz (0,0,0) - (x+,y-,z-)
   	increase = 0, decrease = 0;
    for (short int x = last_x - 1, y = last_y + 1, z = last_z + 1;
         x >= 0 && y < num_ys && z < num_zs && board[x][y][z] == piece;
         --x, ++y, ++z)
      decrease++;
    for (short int x = last_x + 1, y = last_y - 1, z = last_z - 1;
         x < num_xs && y >= 0 && z >= 0 && board[x][y][z] == piece;
         ++x, --y, --z)
      increase++;
    if (increase + 1 + decrease >= JOIN_NUM)
      return piece;

    return none_player_marker;
  }

  double get_result(int current_player_is_moved) const
  {
	
		//#pragma message "Here current_player_is_moved always represents the Next player of state"	

		//current_player_is_moved += (Support_Num_Players-1);
		//current_player_is_moved %= (Support_Num_Players);

    dattest(!has_moves());
    check_invariant();

    auto winner = get_winner();
    if (winner == none_player_marker)
      return 1.0/Support_Num_Players;

    if (winner == player_markers[current_player_is_moved])
      return 1.0;
    else
      return 0.0;
  }

  void print(ostream& out) const
  {
    out << endl;

    for (short int z = 0; z < num_zs; ++z)
    {
      out << "   ";
      for (short int y = 0; y < num_ys - 1; ++y)
      {
        out << y << ' ';
      }
      out << num_ys - 1 << endl;
      for (short int x = 0; x < num_xs; ++x)
      {
        out << x << " |";
        for (short int y = 0; y < num_ys - 1; ++y)
        {
          out << board[x][y][z] << ' ';
        }
        out << board[x][num_ys - 1][z] << "|" << endl;
      }
      out << "  +";
      for (short int y = 0; y < num_ys - 1; ++y)
      {
        out << "--";
      }
      out << "-+" << endl;
    }
    out << player_markers[player_is_moved] << " finished move " << endl << endl;
		out << player_markers[(player_is_moved+1)%Support_Num_Players] << " is to Moving" << endl;
  }

  int player_is_moved;

	const Move getLastMove()const {return {last_x,last_y,last_z};}

private:
  void check_invariant() const
  {
    attest(player_is_moved >= 0 && player_is_moved < Support_Num_Players);
  }

  const short int num_xs, num_ys, num_zs;
  vector<vector<vector<char>>> board;
  vector<Move> reserved_moves;
  short int last_x, last_y, last_z;
};

template<int num_players>
ostream&
operator<<(ostream& out, const GoBangState<num_players>& state)
{
  state.print(out);
  return out;
}

