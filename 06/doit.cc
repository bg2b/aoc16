// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

void solve(int which_way) {
  vector<map<char, int>> frequency_counts;
  string line;
  while (cin >> line) {
    frequency_counts.resize(line.length());
    for (size_t i = 0; i < line.length(); ++i)
      ++frequency_counts[i][line[i]];
  }
  for (auto const &counts : frequency_counts) {
    assert(!counts.empty());
    pair<char, int> intended = *counts.begin();
    for (auto const &i : counts)
      if (which_way * i.second > which_way * intended.second)
        intended = i;
    cout << intended.first;
  }
  cout << '\n';
}

void part1() { solve(+1); }
void part2() { solve(-1); }

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
