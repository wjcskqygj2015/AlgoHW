// Petter Strandmark 2013
// petter.strandmark@gmail.com

#include <algorithm>
#include <iostream>
using namespace std;

#include "../mcts.h"

static int __COUNT__ = 0;

class GoBangState
{
public:
  // static const int DIM=2;
  typedef struct Move
  {
    short int x;
    short int y;
    friend std::istream& operator>>(std::istream& in, struct Move& move)
    {
      in >> move.x >> move.y;
      return in;
    }
    friend std::ostream& operator<<(std::ostream& out, const struct Move& move)
    {
      out << move.x << ' ' << move.y;
      return out;
    }
    bool operator==(const struct Move& ot) const
    {
      return (this->x == ot.x) && (this->y == ot.y);
    }
    bool operator!=(const struct Move& ot) const { return !((*this) == ot); }
    bool operator<(const struct Move& ot) const
    {
      return (this->x < ot.x) || ((this->x == ot.x) && (this->y < ot.y));
    }
  } Move;
  static const Move no_move;

  static const char player_markers[3];

  GoBangState(short int num_rows_ = 6, short int num_cols_ = 7)
    : player_to_move(1), num_rows(num_rows_), num_cols(num_cols_), 
			last_col(-1), last_row(-1)
  {
    board.resize(num_rows, vector<char>(num_cols, player_markers[0]));

    for (short int col = 0; col < num_cols; ++col)
      for (short int row = 0; row < num_rows; ++row)
        reserved_moves.push_back({row, col});
  }

  void do_move(Move move)
  {
    attest(0 <= move.x && move.x < num_rows);
    attest(0 <= move.y && move.y < num_cols);
    attest(board[move.x][move.y] == player_markers[0]);
    check_invariant();

    // int row = num_rows - 1;
    // while (board[row][move] != player_markers[0]) row--;
    board[move.x][move.y] = player_markers[player_to_move];
    last_row = move.x;
    last_col = move.y;

		for(auto it=reserved_moves.cbegin();it!=reserved_moves.cend();it++)
			if(*it==Move({move.x,move.y}))
			{
				reserved_moves.erase(it);
				break;
			}

    player_to_move = 3 - player_to_move;
  }

  template<typename RandomEngine>
  void do_random_move(RandomEngine* engine)
  {
    dattest(has_moves());
    check_invariant();
    std::uniform_int_distribution<short int> moves(0, num_cols - 1);

    while (true)
    {
      Move move = {moves(*engine), moves(*engine)};
      if (board[move.x][move.y] == player_markers[0])
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
    if (winner != player_markers[0])
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

    return reserved_moves;
  }

  char get_winner() const
  {

    const int JOIN_NUM = 5;

    if (last_col < 0)
    {
      return player_markers[0];
    }

    // We only need to check around the last piece played.
    auto piece = board[last_row][last_col];

    // X X X X
    int left = 0, right = 0;
    for (short int col = last_col - 1; col >= 0 && board[last_row][col] == piece;
         --col)
      left++;
    for (short int col = last_col + 1;
         col < num_cols && board[last_row][col] == piece; ++col)
      right++;
    if (left + 1 + right >= JOIN_NUM)
    {
      return piece;
    }

    // X
    // X
    // X
    // X
    int up = 0, down = 0;
    for (short int row = last_row - 1; row >= 0 && board[row][last_col] == piece;
         --row)
      up++;
    for (short int row = last_row + 1;
         row < num_rows && board[row][last_col] == piece; ++row)
      down++;
    if (up + 1 + down >= JOIN_NUM)
    {
      return piece;
    }

    // X
    //  X
    //   X
    //    X
    up = 0;
    down = 0;
    for (short int row = last_row - 1, col = last_col - 1;
         row >= 0 && col >= 0 && board[row][col] == piece; --row, --col)
      up++;
    for (short int row = last_row + 1, col = last_col + 1;
         row < num_rows && col < num_cols && board[row][col] == piece;
         ++row, ++col)
      down++;
    if (up + 1 + down >= JOIN_NUM)
    {
      return piece;
    }

    //    X
    //   X
    //  X
    // X
    up = 0;
    down = 0;
    for (short int row = last_row + 1, col = last_col - 1;
         row < num_rows && col >= 0 && board[row][col] == piece; ++row, --col)
      up++;
    for (short int row = last_row - 1, col = last_col + 1;
         row >= 0 && col < num_cols && board[row][col] == piece; --row, ++col)
      down++;
    if (up + 1 + down >= JOIN_NUM)
    {
      return piece;
    }

    return player_markers[0];
  }

  double get_result(int current_player_to_move) const
  {
    dattest(!has_moves());
    check_invariant();

    auto winner = get_winner();
    if (winner == player_markers[0])
    {
      return 0.5;
    }

    if (winner == player_markers[current_player_to_move])
    {
      return 0.0;
    }
    else
    {
      return 1.0;
    }
  }

  void print(ostream& out) const
  {
    out << endl;
    out << "   ";
    for (short int col = 0; col < num_cols - 1; ++col)
    {
      out << col << ' ';
    }
    out << num_cols - 1 << endl;
    for (short int row = 0; row < num_rows; ++row)
    {
      out << row << " |";
      for (short int col = 0; col < num_cols - 1; ++col)
      {
        out << board[row][col] << ' ';
      }
      out << board[row][num_cols - 1] << "|" << endl;
    }
    out << "  +";
    for (short int col = 0; col < num_cols - 1; ++col)
    {
      out << "--";
    }
    out << "-+" << endl;
    out << player_markers[player_to_move] << " to move " << endl << endl;
  }

  int player_to_move;

private:
  void check_invariant() const
  {
    attest(player_to_move == 1 || player_to_move == 2);
  }

  short int num_rows, num_cols;
  vector<vector<char>> board;
  vector<Move> reserved_moves;
  short int last_col, last_row;
};

ostream&
operator<<(ostream& out, const GoBangState& state)
{
  state.print(out);
  return out;
}

const GoBangState::Move GoBangState::no_move = {-1, -1};
const char GoBangState::player_markers[3] = {'.', 'X', 'O'};
