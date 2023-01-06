// -*- C++ -*-
// g++ -std=c++17 -Wall -g -O -o doit doit.cc md5.c
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <cassert>
#include "md5.h"

using namespace std;

using coord = pair<short, short>;

int const ndir = 4;
int const nmaze = 4;

vector<coord> dirs{ { 0, -1 }, { 0, +1 }, { -1, 0 }, { +1, 0 } };
char const *dir_codes = "UDLR";

coord operator+(coord const &c1, coord const &c2) {
  return { c1.first + c2.first, c1.second + c2.second };
}

bool in_bounds(coord const &c) {
  return c.first >= 0 && c.first < nmaze && c.second >= 0 && c.second < nmaze;
}

string passcode;

string md5(string const &s) {
  md5_state_t pms;
  md5_init(&pms);
  md5_append(&pms, (md5_byte_t *)s.data(), s.length());
  array<unsigned char, 16> digest;
  md5_finish(&pms, digest.data());
  string sig;
  sig.reserve(2 * digest.size());
  auto hex = [](auto c) { return c > 9 ? c - 10 + 'a' : c + '0'; };
  for (auto c : digest) {
    sig.push_back(hex(c >> 4));
    sig.push_back(hex(c & 0xf));
  }
  return sig;
}

struct state {
  coord c;
  string steps;
  array<bool, ndir> open;

  state(coord const &c_, string const &steps_);

  bool done() const { return c.first == nmaze - 1 && c.second == nmaze - 1; }

  list<state> successors() const;
};

state::state(coord const &c_, string const &steps_) : c(c_), steps(steps_) {
  string hash = md5(passcode + steps);
  for (int i = 0; i < ndir; ++i)
    open[i] = hash[i] >= 'b' && in_bounds(c + dirs[i]);
}

list<state> state::successors() const {
  list<state> next;
  for (int i = 0; i < ndir; ++i)
    if (open[i])
      next.emplace_back(c + dirs[i], steps + dir_codes[i]);
  return next;
}

string search(bool find_longest) {
  state initial({ 0, 0 }, "");
  list<state> frontier{ initial };
  string longest;
  while (!frontier.empty()) {
    state s = frontier.front();
    frontier.pop_front();
    if (s.done()) {
      if (!find_longest)
        return s.steps;
      longest = s.steps;
      continue;
    }
    auto next = s.successors();
    frontier.insert(frontier.end(), next.begin(), next.end());
  }
  return longest;
}

void part1() {
  cin >> passcode;
  cout << search(false) << '\n';
}

void part2() {
  cin >> passcode;
  cout << search(true).length() << '\n';
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
