// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

using namespace std;

string next_traps(string const &traps) {
  size_t n = traps.length();
  char left = '.';
  string result;
  for (size_t i = 0; i < n; ++i) {
    char right = i + 1 < n ? traps[i + 1] : '.';
    result.push_back(left == right ? '.' : '^');
    left = traps[i];
  }
  return result;
}

void count_safe(unsigned num_rows) {
  string row;
  cin >> row;
  size_t num_safe = 0;
  auto add_safe = [&] { num_safe += count(row.begin(), row.end(), '.'); };
  add_safe();
  for (unsigned _ = 0; _ < num_rows - 1; ++_) {
    row = next_traps(row);
    add_safe();
  }
  cout << num_safe << '\n';
}

void part1() { count_safe(40); }
void part2() { count_safe(400000); }

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
