// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <map>
#include <functional>
#include <cassert>

using namespace std;

using coord = pair<int, int>;

coord operator+(coord const &xy1, coord const &xy2) {
  return { xy1.first + xy2.first, xy1.second + xy2.second };
}

map<char, coord> deltas =
  { { 'U', { 0, -1 } }, { 'D', { 0, +1 } },
    { 'L', { -1, 0 } }, { 'R', { +1, 0 } } };

void find_code(coord const &start,
               function<bool(coord const &)> valid,
               function<char(coord const &)> to_digit) {
  string line;
  coord at = start;
  while (cin >> line) {
    for (char c : line) {
      assert(deltas.find(c) != deltas.end());
      coord next = at + deltas.find(c)->second;
      if (valid(next))
        at = next;
    }
    cout << to_digit(at);
  }
  cout << '\n';
}

void part1() {
  auto valid = [](coord const &xy) {
                 return abs(xy.first) <= 1 && abs(xy.second) <= 1;
               };
  auto to_digit = [](coord const &xy) {
                    return '1' + (xy.first + 1) + 3 * (xy.second + 1);
                  };
  find_code({ 0, 0 }, valid, to_digit);
}

void part2() {
  auto valid = [](coord const &xy) {
                 return abs(xy.first) + abs(xy.second) <= 2;
               };
  auto to_digit = [](coord const &xy) {
                    char const *digits = ("??1??"
                                          "?234?"
                                          "56789"
                                          "?ABC?"
                                          "??D??");
                    return digits[(xy.first + 2) + 5 * (xy.second + 2)];
                  };
  find_code({ -2, 0 }, valid, to_digit);
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
