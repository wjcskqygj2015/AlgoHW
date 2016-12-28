#ifndef MCTS_HEADER_PETTER
#define MCTS_HEADER_PETTER
//
// Petter Strandmark 2013
// petter.strandmark@gmail.com
//
// Monte Carlo Tree Search for finite games.
//
// Originally based on Python code at
// http://mcts.ai/code/python.html
//
// Uses the "root parallelization" technique [1].
//
// This game engine can play any game defined by a state like this:
/*

class GameState
{
public:
        typedef int Move;
        static const Move no_move = ...

        void do_move(Move move);
        template<typename RandomEngine>
        void do_random_move(*engine);
        bool has_moves() const;
        std::vector<Move> get_moves() const;

        // Returns a value in {0, 0.5, 1}.
        // This should not be an evaluation function, because it will only be
        // called for finished games. Return 0.5 to indicate a draw.
        double get_result(int current_player_is_moved) const;

        int player_is_moved;

        // ...
private:
        // ...
};

*/
//
// See the examples for more details. Given a suitable State, the
// following function (tries to) compute the best move for the
// player to move.
//

namespace MCTS
{
  struct ComputeOptions
  {
    static const int number_of_threads = 8;
    int max_iterations;
    bool verbose;

    ComputeOptions() : max_iterations(10000), verbose(false) {}
  };

  template<typename State>
  typename State::Move compute_move(
    const State root_state, const ComputeOptions options = ComputeOptions());
}
//
//
// [1] Chaslot, G. M. B., Winands, M. H., & van Den Herik, H. J. (2008).
//     Parallel monte-carlo tree search. In Computers and Games (pp.
//     60-71). Springer Berlin Heidelberg.
//

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace MCTS
{
  using std::cerr;
  using std::endl;
  using std::vector;
  using std::size_t;

// static void check(bool expr, const char* message);
// static void assertion_failed(const char* expr, const char* file, int line);

#define attest(expr)                                                           \
  do                                                                           \
  {                                                                            \
  } while (0)
#define dattest(expr)                                                          \
  do                                                                           \
  {                                                                            \
  } while (0)

  template<typename State, typename = void>
  struct getSupportNumPlayers
  {
    static const int value = 2;
  };

  template<typename State>
  struct getSupportNumPlayers<
    State, decltype(typename std::enable_if<std::is_integral<decltype(
                      State::Support_Num_Players)>::value>::type())>
  {
    static const int value = State::Support_Num_Players;

    static const int only_for_test = 0;
  };
  //
  // This class is used to build the game tree. The root is created by the users
  // and
  // the rest of the tree is created by add_node.
  //
  template<typename State>
  class Node
  {
  public:
    typedef typename State::Move Move;

    Node(const State& state);
    ~Node();

    bool has_untried_moves() const;
    template<typename RandomEngine>
    Move get_untried_move(RandomEngine* engine) const;
    Node* best_child() const;

    bool has_children() const { return !children.empty(); }

    Node* select_child_UCT() const;
    Node* add_child(const Move& move, const State& state);
    Node* get_child_from_move_and_prune_other_child_and_unchina_all(
      const Move& move);
    void add_child_and_wont_add_anymore(const Move& move, const State& state);
    void prune_all_childs_without_one(const Node<State>* lucky_one_child);
    void update(double result[], double visit);

    void set_parent_nullptr();

    std::string to_string() const;
    std::string tree_to_string(int max_depth = 1000000, int indent = 0) const;

    const Move move;
    Node* parent;
    const int player_is_moved;

    static const int Support_Num_Players = getSupportNumPlayers<State>::value;
    // std::atomic<double> wins;
    // std::atomic<int> visits;
    double wins[Support_Num_Players];
    double visits;

    std::vector<Move> moves;
    std::vector<Node*> children;

  private:
    Node(const State& state, const Move& move, Node* parent);

    std::string indent_string(int indent) const;

    Node(const Node&);
    Node& operator=(const Node&);

    double UCT_score;
  };

  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  template<typename State>
  Node<State>::Node(const State& state)
    : move(State::no_move), parent(nullptr),
      player_is_moved(state.player_is_moved), visits(0),
      moves(state.get_moves()), UCT_score(0)
  {
    for (int i = 0; i < Support_Num_Players; i++)
      wins[i] = 0;
  }

  template<typename State>
  Node<State>::Node(const State& state, const Move& move_, Node* parent_)
    : move(move_), parent(parent_), player_is_moved(state.player_is_moved),
      visits(0), moves(state.get_moves()), UCT_score(0)
  {
    for (int i = 0; i < Support_Num_Players; i++)
      wins[i] = 0;
  }

  template<typename State>
  Node<State>::~Node()
  {
    for (auto child : children)
    {
      delete child;
    }
  }

  template<typename State>
  bool Node<State>::has_untried_moves() const
  {
    return !moves.empty();
  }

  template<typename State>
  void Node<State>::set_parent_nullptr()
  {
    this->parent = nullptr;
  }

  template<typename State>
  template<typename RandomEngine>
  typename State::Move Node<State>::get_untried_move(RandomEngine* engine) const
  {
    attest(!moves.empty());
    std::uniform_int_distribution<std::size_t> moves_distribution(
      0, moves.size() - 1);
    return moves[moves_distribution(*engine)];
  }

  template<typename State>
  Node<State>* Node<State>::best_child() const
  {
    attest(moves.empty());
    attest(!children.empty());

    return *std::max_element(
      children.begin(), children.end(),
      [](Node* a, Node* b) { return a->visits < b->visits; });
    ;
  }

  template<typename State>
  Node<State>* Node<State>::select_child_UCT() const
  {
    attest(!children.empty());
    for (auto child : children)
    {
      // double square_sum = 0.0;
      // double linear_sum = 0.0;
      // for (int i =
      //(child->player_is_moved + 1) % Node<State>::Support_Num_Players;
      // i != child->player_is_moved;
      // i = (i + 1) % Node<State>::Support_Num_Players)
      //{
      // double temp = double(child->wins[i]) / double(child->visits);
      // square_sum += temp * temp;
      // linear_sum += temp;
      //}
      // square_sum -= (linear_sum * linear_sum);
      // square_sum /= (Node<State>::Support_Num_Players - 1);

      double max_non_me = 0.0;
      for (int i =
             (child->player_is_moved + 1) % Node<State>::Support_Num_Players;
           i != child->player_is_moved;
           i = (i + 1) % Node<State>::Support_Num_Players)
      {
        double temp = double(child->wins[i]) / double(child->visits);
        if (temp > max_non_me)
          max_non_me = temp;
      }
      child->UCT_score =
        (double(child->wins[child->player_is_moved]) / double(child->visits) +
         (1.0 - max_non_me) / (Node<State>::Support_Num_Players) +
         std::sqrt(4.0 * std::log(double(this->visits)) / child->visits));
    }

    return *std::max_element(
      children.begin(), children.end(),
      [](Node* a, Node* b) { return a->UCT_score < b->UCT_score; });
  }

  template<typename State>
  Node<State>* Node<State>::add_child(const Move& move, const State& state)
  {
    auto node = new Node(state, move, this);
    children.push_back(node);
    attest(!children.empty());

    auto itr = moves.begin();
    for (; itr != moves.end() && *itr != move; ++itr)
      ;
    attest(itr != moves.end());
    moves.erase(itr);
    return node;
  }

  template<typename State>
  void Node<State>::add_child_and_wont_add_anymore(const Move& move,
                                                   const State& state)
  {
    auto node = new Node(state, move, this);

    attest(children.empty());
    children.push_back(node);
    attest(!children.empty());

    moves.erase(moves.begin(), moves.end());
  }

  template<typename State>
  void Node<State>::prune_all_childs_without_one(
    const Node<State>* lucky_one_child)
  {

    if ((lucky_one_child == NULL) && (children.size() == 1) &&
        (moves.size() == 0) && (children[0] == lucky_one_child))
      return;

    auto itr = children.cbegin();
    for (; itr != children.cend() && *itr != lucky_one_child; ++itr)
      delete *itr;

    attest(itr != children.cend());

    children.erase(children.cbegin(), itr);

    for (itr = children.cbegin() + 1; itr != children.cend(); itr++)
      delete *itr;
    children.erase(children.cbegin() + 1, children.cend());

    moves.erase(moves.cbegin(), moves.cend());
  }

  template<typename State>
  Node<State>*
  Node<State>::get_child_from_move_and_prune_other_child_and_unchina_all(
    const Move& move)
  {
    Node<State>* selected_child = NULL;
    auto itr = children.cbegin();
    for (; itr != children.cend() && (*itr)->move != move; ++itr)
      delete *itr;

    selected_child = (*itr);
    itr++;

    for (; itr != children.cend(); itr++)
      delete *itr;
    // Release selected_child's property without release the selected children
    children.erase(children.cbegin(), children.cend());
    return selected_child;
  }

  template<typename State>
  void Node<State>::update(double result[], double visit)
  {
    visits += visit;

    for (int i = 0; i < Support_Num_Players; i++)
      wins[i] += result[i];
    // double my_wins = wins.load();
    // while ( ! wins.compare_exchange_strong(my_wins, my_wins + result));
  }

  template<typename State>
  std::string Node<State>::to_string() const
  {
    std::stringstream sout;
    sout << "["
         << "P" << 3 - player_is_moved << " "
         << "M:" << move << " "
         << "W/V: " << wins << "/" << visits << " "
         << "U: " << moves.size() << "]\n";
    return sout.str();
  }

  template<typename State>
  std::string Node<State>::tree_to_string(int max_depth, int indent) const
  {
    if (indent >= max_depth)
    {
      return "";
    }

    std::string s = indent_string(indent) + to_string();
    for (auto child : children)
    {
      s += child->tree_to_string(max_depth, indent + 1);
    }
    return s;
  }

  template<typename State>
  std::string Node<State>::indent_string(int indent) const
  {
    std::string s = "";
    for (int i = 1; i <= indent; ++i)
    {
      s += "| ";
    }
    return s;
  }

  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  template<typename State>
  void human_do_move(const State& root_state);
  template<typename State>
  typename State::Move compute_move(const State root_state,
                                    const ComputeOptions options);

  namespace EXPR_DECAY_DOUBLE_ARRAY
  {
    template<template<size_t> class Func, int... args>
    struct ArrayHolder
    {
      static constexpr const double data[sizeof...(args)] = {
        Func<args>::output...};
    };

    template<template<size_t> class Func, int... args>
    constexpr const double ArrayHolder<Func, args...>::data[sizeof...(args)];

    template<size_t N, template<size_t> class Func, template<size_t> class F,
             int... args>
    struct generate_array_impl
    {
      typedef typename generate_array_impl<N - 1, Func, F, F<N>::value,
                                           args...>::result result;
    };

    template<template<size_t> class Func, template<size_t> class F, int... args>
    struct generate_array_impl<0, Func, F, args...>
    {
      typedef ArrayHolder<Func, F<0>::value, args...> result;
    };

    template<size_t N, template<size_t> class Func, template<size_t> class F>
    struct generate_array
    {
      typedef typename generate_array_impl<N - 1, Func, F>::result result;
    };

    template<size_t index>
    struct OriginGenerator
    {
      static constexpr const int value = index;
    };

    template<size_t input>
    struct ExprDecayGenerator
    {
      static constexpr const double decay_per_times = 0.95;
      static constexpr const double output = pow(decay_per_times, input);
    };
    // constexpr double fun(size_t x) { return pow(0.95,x); }
    template<size_t index>
    struct EXPR_DECAY_DOUBLE_ARRAY
    {
      typedef typename generate_array<index, ExprDecayGenerator,
                                      OriginGenerator>::result result;
      // static constexpr const double (&markers)[index] = result::data;
    };
  }

  template<typename State>
  class Preserve_State
  {
  public:
    friend void human_do_move<State>(const State& root_state);

    friend typename State::Move compute_move<State>(
      const State root_state, const ComputeOptions options);

    static constexpr int max_depth = 60;
    static constexpr const double (&decay_value)[max_depth] =
      EXPR_DECAY_DOUBLE_ARRAY::EXPR_DECAY_DOUBLE_ARRAY<max_depth>::result::data;

  private:
    static Node<State>* last_state[ComputeOptions::number_of_threads];

    static void do_last_move_for_preserve_state(const State& root_state)
    {
      for (int i = 0; i < ComputeOptions::number_of_threads; i++)
      {
        Node<State>* temp = Preserve_State<State>::last_state[i];
        Preserve_State<State>::last_state[i] =
          Preserve_State<State>::last_state[i]
            ->get_child_from_move_and_prune_other_child_and_unchina_all(
              root_state.getLastMove());
        if (Preserve_State<State>::last_state[i] == NULL)
          Preserve_State<State>::last_state[i] = new Node<State>(root_state);
        Preserve_State<State>::last_state[i]->set_parent_nullptr();
        delete temp;
      }
    }
  };

  template<typename State>
  Node<State>*
    Preserve_State<State>::last_state[ComputeOptions::number_of_threads] = {};

  template<typename State>
  void human_do_move(const State& root_state)
  {
    Preserve_State<State>::do_last_move_for_preserve_state(root_state);
  }

  template<typename State>
  const Node<State>* compute_tree(const State root_state,
                                  Node<State>* last_state,
                                  const ComputeOptions options)
  {
    // std::mt19937_64 random_engine(initial_seed);
    std::random_device random_engine;

    attest(options.max_iterations >= 0);

    auto root = last_state;

    // const int max_depth = Preserve_State<State>::max_d
    // static const int Support_Num_Players =
    // getSupportNumPlayers<State>::value;

    // const int for_test = getSupportNumPlayers<State>::only_for_test;
    double result[Node<State>::Support_Num_Players] = {};

    for (int iter = 1;
         iter <= options.max_iterations || options.max_iterations < 0; ++iter)
    {
      auto node = root;
      State state = root_state;

      // Select a path through the tree to a leaf node.
      while (!node->has_untried_moves() && node->has_children())
      {
        node = node->select_child_UCT();
        state.do_move(node->move);
      }

      // If we are not already at the final state, expand the
      // tree with a new node and move there.
      if (node->has_untried_moves())
      {
        auto move = node->get_untried_move(&random_engine);
        state.do_move(move);
        node = node->add_child(move, state);
      }

      // We apply Pruning Techniques while we are playing this games
      // Of course, when we find the non-random step caused terminal
      // It is easily to pruning other possibility for its parents

      int need_to_end_count = 0;
      // We now play randomly until the game ends.
      while (state.has_moves())
      {
        need_to_end_count++;
        state.do_random_move(&random_engine);
      }

      bool has_winner = state.has_winner();
      // need_to_end_count represents that the winners are cause by itself
      // Of course the result is that whatever happened we would choose the
      // action when we are in parent state
      if (need_to_end_count == 0 && has_winner)
      {
        auto parent_node = node->parent;
        parent_node->prune_all_childs_without_one(node);
        for (int i =
               (node->player_is_moved + 1) % Node<State>::Support_Num_Players;
             i != node->player_is_moved;
             i = (i + 1) % Node<State>::Support_Num_Players)
          parent_node->wins[i] = 0;
        parent_node->wins[node->player_is_moved] = parent_node->visits;
      }

      // Here the pruning techniques base on such facts
      // when we just choose one random and cause the winner
      // means that when I was in this state ( after I chose untries_move before
      // start random)
      // and choose this state is unwise for me,
      // However, we could suppose that enemy has this possiblity when there is
      // no other move
      // So we could not just pruning from this state's parent
      // But We could just pruning its child let only thin
      if (need_to_end_count == 1 && has_winner)
      {
        auto last_move = state.getLastMove();
        node->add_child_and_wont_add_anymore(last_move, state);
        int next_player =
          (node->player_is_moved + 1) % Node<State>::Support_Num_Players;
        for (int i = (next_player + 1) % Node<State>::Support_Num_Players;
             i != next_player; i = (i + 1) % Node<State>::Support_Num_Players)
          node->wins[i] = 0;
        node->wins[next_player] = node->visits;
      }

      //if (need_to_end_count >= Preserve_State<State>::max_depth)
        //need_to_end_count = Preserve_State<State>::max_depth - 1;
      // We have now reached a final state. Backpropagate the result
      // up the tree to the root node.

      // for (int i = 0; i < Node<State>::Support_Num_Players; i++)
      // result[i] = state.get_result(i) *
      // Preserve_State<State>::decay_value[need_to_end_count];

      // double visit = Preserve_State<State>::decay_value[need_to_end_count];
      for (int i = 0; i < Node<State>::Support_Num_Players; i++)
        result[i] = state.get_result(i);

      double visit = 1.0;

      while (node != nullptr)
      {
        node->update(result, visit);
        node = node->parent;
      }
    }

    return root;
  }

  template<typename State>
  typename State::Move compute_move(const State root_state,
                                    const ComputeOptions options)
  {
    using namespace std;

    // Will support more players later.
    // attest(root_state.player_is_moved == 1 || root_state.player_is_moved ==
    // 2);

    auto moves = root_state.get_moves();
    attest(moves.size() > 0);
    if (moves.size() == 1)
    {
      return moves[0];
    }

    //------------------------
    static bool if_is_first = true;
    // static Node<State>* last_state[options.number_of_threads];
    if (if_is_first == true)
    {
      for (int i = 0; i < options.number_of_threads; i++)
        Preserve_State<State>::last_state[i] = new Node<State>(root_state);
      if_is_first = false;
    }
    else
    {
      Preserve_State<State>::do_last_move_for_preserve_state(root_state);
    }

    //------------------------------------
    // Start all jobs to compute trees.
    vector<future<const Node<State>*>> root_futures;
    ComputeOptions job_options = options;
    job_options.verbose = false;
    for (int t = 0; t < options.number_of_threads; ++t)
    {
      auto func = [t, &root_state, &job_options]() -> const Node<State>* {
        return compute_tree(root_state, Preserve_State<State>::last_state[t],
                            job_options);
      };

      root_futures.push_back(std::async(std::launch::async, func));
    }

    // Collect the results.
    vector<const Node<State>*> roots;
    for (int t = 0; t < options.number_of_threads; ++t)
    {
      roots.push_back(root_futures[t].get());
    }

    // Merge the children of all root nodes.
    map<typename State::Move, double> visits;
    map<typename State::Move, double> wins[Node<State>::Support_Num_Players];
    long long games_played = 0;
    for (int t = 0; t < options.number_of_threads; ++t)
    {
      auto root = roots[t];
      games_played += root->visits;
      for (auto child = root->children.cbegin(); child != root->children.cend();
           ++child)
      {
        visits[(*child)->move] += (*child)->visits;
        for (int i = 0; i < Node<State>::Support_Num_Players; i++)
          wins[i][(*child)->move] += (*child)->wins[i];
      }
    }

    const int player_is_to_move =
      (root_state.player_is_moved + 1) % Node<State>::Support_Num_Players;

    // Find the node with the highest score.
    double best_score = -1;
    typename State::Move best_move = typename State::Move();
    for (auto itr : visits)
    {
      auto move = itr.first;
      double v = itr.second;
      double w = wins[player_is_to_move][move];
      double max_no_me_wins = 0.0;
      for (int i = (player_is_to_move + 1) % Node<State>::Support_Num_Players;
           i != player_is_to_move;
           i = (i + 1) % Node<State>::Support_Num_Players)
        if (wins[i][move] > max_no_me_wins)
          max_no_me_wins = wins[i][move];
      // Expected success rate assuming a uniform prior (Beta(1, 1)).
      // https://en.wikipedia.org/wiki/Beta_distribution
      double expected_success_rate = (w + 1) / (v + 2) +
                                     (1.0 - (max_no_me_wins + 1) / (v + 2)) /
                                       Node<State>::Support_Num_Players;
      if (expected_success_rate > best_score)
      {
        best_move = move;
        best_score = expected_success_rate;
      }

      if (options.verbose)
      {
        cerr << "Move: " << itr.first << " (" << setw(4) << right
             << 100.0 * v / double(games_played) << "% visits)"
             << " (" << setw(4) << w << ' ' << v << ' ' << (w / v) * 100.0
             << "% wins)" << endl;
      }
    }

    if (options.verbose)
    {
      auto best_wins = wins[player_is_to_move][best_move];
      auto best_visits = visits[best_move];
      cerr << "----" << endl;
      cerr << "Best: " << best_move << " ("
           << 100.0 * best_visits / double(games_played) << "% visits)"
           << " (" << 100.0 * best_wins / best_visits << "% wins)" << endl;
    }

    return best_move;
  }

  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  // static void check(bool expr, const char* message)
  //{
  // if (!expr)
  //{
  // throw std::invalid_argument(message);
  //}
  //}

  // static void assertion_failed(const char* expr, const char* file_cstr,
  // int line)
  //{
  // using namespace std;

  //// Extract the file name only.
  // string file(file_cstr);
  // auto pos = file.find_last_of("/\\");
  // if (pos == string::npos)
  //{
  // pos = 0;
  //}
  // file = file.substr(pos + 1); // Returns empty string if pos + 1 == length.

  // stringstream sout;
  // sout << "Assertion failed: " << expr << " in " << file << ":" << line
  //<< ".";
  // throw runtime_error(sout.str().c_str());
  //}
}

#endif
