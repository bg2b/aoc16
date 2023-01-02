// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <set>
#include <optional>
#include <cassert>

using namespace std;

struct factory {
  // For distribution and holding and giving, negative means an
  // output, non-negative is a bot.  output 0 => -1, output 1 => -2,
  // etc.
  map<int, pair<int, int>> distribution;
  map<int, int> holding;
  // Which bot compared two values
  map<pair<int, int>, int> compared;

  factory();

  // Give a chip to a bot, or distribute it to an output
  void give(int value, int bot_or_output);

  // Which bot compared two chips?
  int who_compared(int low, int high) const;
  // What value chip went to the output?
  int output_value(int output) const;
};

factory::factory() {
  string instr;
  list<pair<int, int>> initial;
  while (cin >> instr) {
    string _;
    if (instr == "value") {
      int val, bot;
      cin >> val >> _ >> _ >> _ >> bot;
      initial.emplace_back(val, bot);
      continue;
    }
    string low_type, high_type;
    int bot, low, high;
    cin >> bot >> _ >> _ >> _ >> low_type >> low >>
      _ >> _ >> _ >> high_type >> high;
    // Encode outputs as negative
    if (low_type == "output")
      low = -1 - low;
    if (high_type == "output")
      high = -1 - high;
    assert(distribution.find(bot) == distribution.end());
    distribution.emplace(bot, make_pair(low, high));
  }
  for (auto const &i : initial)
    give(i.first, i.second);
}

void factory::give(int val, int bot_or_output) {
  auto p = holding.find(bot_or_output);
  if (p == holding.end()) {
    // Just give the value to the bot or the output
    holding.emplace(bot_or_output, val);
    return;
  }
  // Can't give multiple values to one output
  assert(bot_or_output >= 0);
  // Bot redistributes low and high value chips
  int bot = bot_or_output;
  int low = min(val, p->second);
  int high = max(val, p->second);
  compared.emplace(make_pair(low, high), bot);
  holding.erase(p);
  auto q = distribution.find(bot);
  assert(q != distribution.end());
  give(low, q->second.first);
  give(high, q->second.second);
}

int factory::who_compared(int low, int high) const {
  auto p = compared.find({ low, high });
  assert(p != compared.end());
  return p->second;
}

int factory::output_value(int output) const {
  auto p = holding.find(-1 - output);
  assert(p != holding.end());
  return p->second;
}

void part1() { cout << factory().who_compared(17, 61) << '\n'; }

void part2() {
  factory f;
  cout << f.output_value(0) * f.output_value(1) * f.output_value(2) << '\n';
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
