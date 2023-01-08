// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <cctype>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

struct ducts {
  // Locations of '0', '1', ...
  vector<coord> locs;
  // The map
  vector<string> rows;
  // Pairwise shortest paths between locs
  vector<vector<int>> shortest_paths;

  // Construct from stdin
  ducts();

  char at(coord const &c) const { return rows[c.first][c.second]; }

  // Fill in shortest_paths
  void compute_paths();

  // Find shortest tour with/without return to start
  int shortest_tour(bool go_back) const;
};

ducts::ducts() {
  string row;
  while (getline(cin, row)) {
    string::size_type pos = 0;
    while ((pos = row.find_first_of("0123456789", pos)) != string::npos) {
      unsigned index = row[pos] - '0';
      coord c{ rows.size(), pos };
      if (index >= locs.size())
        locs.resize(index + 1);
      locs[index] = c;
      ++pos;
    }
    rows.push_back(row);
    assert(row.length() == rows.front().length());
  }
  compute_paths();
}

void ducts::compute_paths() {
  shortest_paths.resize(locs.size());
  for (size_t start = 0; start < locs.size(); ++start) {
    // Generic breadth-first search starting at locs[start]
    shortest_paths[start].resize(locs.size(), 0);
    set<coord> visited;
    list<pair<coord, int>> frontier;
    auto visit = [&](coord const &c, int steps) {
                   if (at(c) == '#' || visited.count(c))
                     return;
                   visited.insert(c);
                   frontier.emplace_back(c, steps);
                 };
    visit(locs[start], 0);
    while (!frontier.empty()) {
      auto [c, steps] = frontier.front();
      frontier.pop_front();
      char ch = at(c);
      assert(ch != '#');
      if (isdigit(ch))
        shortest_paths[start][ch - '0'] = steps;
      static vector<coord> dirs{ { -1, 0 }, { +1, 0 }, { 0, -1 }, { 0, +1 } };
      for (auto const &dir : dirs)
        visit(c + dir, steps + 1);
    }
  }
}

int ducts::shortest_tour(bool go_back) const {
  // The number of locations is so small that it's simplest to just
  // brute force through all permutations
  vector<unsigned> order;
  for (unsigned i = 0; i < locs.size(); ++i)
    order.push_back(i);
  optional<int> best_length;
  do {
    int len = 0;
    for (unsigned i = 0; i + 1 < order.size(); ++i)
      len += shortest_paths[order[i]][order[i + 1]];
    if (go_back)
      len += shortest_paths[order.back()][0];
    best_length = min(best_length.value_or(len), len);
  } while (next_permutation(order.begin() + 1, order.end()));
  return *best_length;
}

void solve(bool go_back) { cout << ducts().shortest_tour(go_back) << '\n'; }

void part1() { solve(false); }
void part2() { solve(true); }

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
