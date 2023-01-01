// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <functional>
#include <set>
#include <cstdlib>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &xy1, coord const &xy2) {
  return { xy1.first + xy2.first, xy1.second + xy2.second };
}

void walk(coord &xy, function<void()> fn) {
  coord dir{ 0, 1 };
  auto left = [&]() { swap(dir.first, dir.second); dir.first = -dir.first; };
  string step;
  while (cin >> step) {
    if (step.back() == ',')
      step.pop_back();
    assert(!step.empty());
    assert(step.front() == 'L' || step.front() == 'R');
    left();
    if (step.front() == 'R') {
      // Two wrongs don't make a right, but three lefts do
      left();
      left();
    }
    int num_steps = atoi(step.c_str() + 1);
    while (num_steps-- > 0) {
      xy = xy + dir;
      fn();
    }
  }
}

void part1() {
  coord xy{ 0, 0 };
  walk(xy, [](){});
  cout << abs(xy.first) + abs(xy.second) << '\n';
}

void part2() {
  coord xy{ 0, 0 };
  set<coord> visited;
  visited.insert(xy);
  optional<coord> ans;
  walk(xy, [&]() {
             if (visited.count(xy) && !ans)
               ans = xy;
             visited.insert(xy);
           });
  assert(ans);
  cout << abs(ans->first) + abs(ans->second) << '\n';
}

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
