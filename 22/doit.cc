// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <queue>
#include <optional>
#include <algorithm>
#include <cstdlib>
#include <cassert>

using namespace std;

// used, avail
using node = pair<unsigned, unsigned>;
// x, y
using index = pair<int, int>;

index operator+(index const &i1, index const &i2) {
  return { i1.first + i2.first, i1.second + i2.second };
}

int manhattan(index const &i1, index const &i2) {
  return abs(i1.first - i2.first) + abs(i1.second - i2.second);
}

// The initial configuration with full details of used and avail
struct grid {
  vector<vector<node>> nodes;

  // Construct from stdin
  grid();

  node at(index const &xy) const { return nodes[xy.second][xy.first]; };

  // Indexes for all the nodes
  vector<index> all_nodes() const;
};

grid::grid() {
  auto add = [&](unsigned x, unsigned y, node const &n) {
               while (y >= nodes.size())
                 nodes.emplace_back(vector<node>());
               while (x >= nodes[y].size())
                 nodes[y].emplace_back(0, 0);
               nodes[y][x] = n;
             };
  string line;
  while (getline(cin, line)) {
    if (line[0] != '/')
      continue;
    stringstream ss(line);
    string name, size_, used_, avail_, pct;
    ss >> name >> size_ >> used_ >> avail_ >> pct;
    unsigned x = atoi(name.c_str() + name.find('x') + 1);
    unsigned y = atoi(name.c_str() + name.find('y') + 1);
    unsigned size = stoi(size_);
    unsigned used = stoi(used_);
    unsigned avail = stoi(avail_);
    assert(size == used + avail);
    add(x, y, { used, avail });
  }
  for (auto const &row : nodes)
    assert(row.size() == nodes.front().size());
}

vector<index> grid::all_nodes() const {
  vector<index> result;
  for (unsigned y = 0; y < nodes.size(); ++y)
    for (unsigned x = 0; x < nodes[y].size(); ++x)
      result.emplace_back(x, y);
  return result;
}

void part1() {
  // Could try to be clever and do some sorting by used and avail (so
  // as to avoid a quadratic scan), but there's no point.  The whole
  // purpose of this part is likely to make you realize that things
  // map into the sort of scenario as in the description of part 2.
  grid g;
  auto indexes = g.all_nodes();
  unsigned ans = 0;
  for (auto const &ia : indexes) {
    node A = g.at(ia);
    if (A.first == 0)
      continue;
    for (auto const &ib : indexes)
      if (ib != ia && A.first <= g.at(ib).second)
        ++ans;
  }
  cout << ans << '\n';
}

// Abstract version of the grid, as discussed in the part 2
// description
struct abstract_grid {
  // Nearly full nodes with lots of data that are too big to move
  // (basically obstacles)
  set<index> unmovable;
  // The initial position of the empty node
  index hole;
  // Where's the data to fetch?
  index goal{ 0, 0 };
  // Maximum coordinates
  index corner{ 0, 0 };

  abstract_grid(grid const &g);
};

abstract_grid::abstract_grid(grid const &g) {
  // Find maximum coordinates and locations of hole and goal
  auto indexes = g.all_nodes();
  bool found_hole = false;
  for (auto const &i : indexes) {
    corner.first = max(corner.first, i.first);
    corner.second = max(corner.second, i.second);
    if (i.second == 0)
      goal.first = max(goal.first, i.first);
    node n = g.at(i);
    if (n.first == 0) {
      assert(!found_hole);
      found_hole = true;
      hole = i;
    }
  }
  assert(found_hole);
  auto hole_avail = g.at(hole).second;
  // Find unmovable nodes, make sure that the hole can fit movable
  // nodes, get bounds of used and avail
  unsigned max_nonhole_avail = 0;
  unsigned min_movable_used = hole_avail + 1;
  for (auto const &i : indexes) {
    if (i == hole)
      continue;
    node n = g.at(i);
    max_nonhole_avail = max(max_nonhole_avail, n.second);
    if (n.first > hole_avail)
      unmovable.insert(i);
    else
      min_movable_used = min(min_movable_used, n.first);
  }
  // No other nodes should be able to act like the hole
  assert(min_movable_used > max_nonhole_avail);
  // Controllable corner and goal should be movable
  assert(!unmovable.count({ 0, 0 }) && !unmovable.count(goal));
}

// Search state for shortest sequence
struct state {
  // Current location of hole
  index hole;
  // Current location of goal
  index goal;
  // Number of steps taken
  unsigned steps{0};
  // Lower bound on the number of steps at solution
  unsigned bound;

  // Initial state
  state(abstract_grid const &absg);
  // Constructor for making successors
  state(index const &hole_, index const &goal_, unsigned steps_);

  // Desired data obtained?
  bool done() const { return goal == index{ 0, 0 }; }
  // Compute the lower bound
  void compute_bound();

  // List of all legal successors.  The hole moves and tries to drag
  // the goal to { 0, 0 }, but it can't go out of bounds and can't
  // move to any of absg.unmovable.
  list<state> successors(abstract_grid const &absg) const;
};

state::state(abstract_grid const &absg) : hole(absg.hole), goal(absg.goal) {
  compute_bound();
}

state::state(index const &hole_, index const &goal_, unsigned steps_) :
  hole(hole_), goal(goal_), steps(steps_) {
  compute_bound();
}

void state::compute_bound() {
  bound = steps;
  if (done())
    return;
  // Hole moves next to goal to start dragging it, taking one step per
  // unit of Manhattan distance
  bound += max(manhattan(hole, goal) - 1, 0);
  // Since the idea is to get goal to { 0, 0 }, the goal's coordinates
  // tell how far it has to go in each direction.  For the initial
  // part of the dragging where there's both horizontal and vertical
  // shifting, this sort of dance is possible (G is goal, _ is hole,
  // everything else is a dot, the goal is being dragged up and left):
  // . .     . .    . _    _ .    G .
  // _ G  => G _ => G . => G . => _ .
  // . .     . .    . .    . .    . .
  // That's two steps of the hole per unit of (Manhattan) distance
  // aside from perhaps the first one
  int min_offset = min(goal.first, goal.second);
  int diagonal = max(min_offset - 1, 0);
  bound += 2 * diagonal;
  // Once the goal needs to be moved just in one direction, this kind
  // of circulation is the best possible
  // . _ G => . G _ => . G . => . G . => . G . => _ G .
  // . . .    . . .    . . _    . _ .    _ . .    . . .
  // That's four steps of the hole per unit of distance (aside from
  // possibly the first)
  int remaining_offset = max(goal.first, goal.second) - min_offset;
  int straight = max(remaining_offset - 1, 0);
  bound += 4 * straight;
}

list<state> state::successors(abstract_grid const &absg) const {
  list<state> result;
  static vector<index> dirs{ { -1, 0 }, { +1, 0 }, { 0, -1 }, { 0, +1 } };
  for (auto const &dir : dirs) {
    index next_hole = hole + dir;
    if (next_hole.first < 0 || next_hole.first > absg.corner.first ||
        next_hole.second < 0 || next_hole.second > absg.corner.second)
      // Out of bounds
      continue;
    if (absg.unmovable.count(next_hole))
      // Can't move this node
      continue;
    index next_goal = goal;
    if (next_hole == next_goal)
      // Swapping with the goal
      next_goal = hole;
    result.emplace_back(next_hole, next_goal, steps + 1);
  }
  return result;
}

bool operator<(state const &s1, state const &s2) {
  return s1.goal != s2.goal ? s1.goal < s2.goal : s1.hole < s2.hole;
}

unsigned search(grid const &g) {
  abstract_grid absg(g);
  set<state> seen;
  auto comp_bound = [](state const &s1, state const &s2) {
                      return s1.bound > s2.bound;
                    };
  priority_queue<state, vector<state>, decltype(comp_bound)> q(comp_bound);
  auto visit = [&](state const &s) {
                 auto p = seen.find(s);
                 if (p != seen.end()) {
                   if (p->steps <= s.steps)
                     // Already seen with no more steps than now
                     return;
                   // Need to update with new smaller number of steps
                   seen.erase(p);
                 }
                 seen.emplace(s);
                 q.push(s);
               };
  visit(state(absg));
  optional<unsigned> min_steps;
  while (!q.empty()) {
    state s = q.top();
    q.pop();
    if (s.done()) {
      // Data obtained
      min_steps = min(min_steps.value_or(s.steps), s.steps);
      continue;
    }
    if (min_steps && s.bound >= *min_steps)
      // Cut off
      continue;
    for (auto const &next : s.successors(absg))
      visit(next);
  }
  assert(min_steps);
  return *min_steps;
}

void part2() { cout << search(grid()) << '\n'; }

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " partnum < input\n";
    exit(1);
  }
  if (*argv[1] == '1')
    part1();
  else
    part2();
  return 0;
}
