// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <list>
#include <set>
#include <optional>
#include <cassert>

using namespace std;

using num = long;
using coord = pair<num, num>;

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

inline int parity(num n) { return __builtin_parityl(n); }

struct maze {
  num favorite;

  maze(num favorite_) : favorite(favorite_) {}

  char at(coord const &c) const;
};

char maze::at(coord const &c) const {
  auto [x, y] = c;
  if (x < 0 || y < 0)
    return '#';
  num n = x * x + 3 * x + 2 * x * y + y + y * y + favorite;
  return parity(n) == 0 ? '.' : '#';
}

void bfs(coord const &target, unsigned max_steps) {
  num favorite;
  cin >> favorite;
  maze mz(favorite);
  assert(mz.at({ 1, 1 }) == '.');
  using state = pair<unsigned, coord>;
  set<coord> reached;
  list<state> frontier;
  auto visit = [&](unsigned steps, coord const &c) {
                 if (reached.count(c))
                   return;
                 reached.insert(c);
                 frontier.emplace_back(steps, c);
               };
  visit(0, { 1, 1 });
  optional<unsigned> min_steps;
  while (!frontier.empty()) {
    auto [steps, c] = frontier.front();
    frontier.pop_front();
    if (c == target) {
      min_steps = steps;
      break;
    }
    if (steps >= max_steps)
      continue;
    static list<coord> dirs{ { +1, 0 }, { 0, +1 }, { -1, 0 }, { 0, -1 } };
    for (coord const &delta : dirs) {
      coord next = c + delta;
      if (mz.at(next) == '.')
        visit(steps + 1, next);
    }
  }
  if (min_steps)
    cout << *min_steps << '\n';
  else
    cout << reached.size() << '\n';
}

void part1() { bfs({ 31, 39 }, 9999); }
void part2() { bfs({ 9999, 9999 }, 50); }

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
