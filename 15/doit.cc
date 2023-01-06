// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <numeric>
#include <utility>
#include <cassert>

using namespace std;

// A group of discs is first effectively aligned at time first, with a
// period of second.  ("Effectively" means this accounts for the delay
// between button press and reaching each disc, i.e., the time means
// when the button is pressed.)
using discs = pair<unsigned, unsigned>;

// Align two groups of discs (simple sieving method)
discs align(discs const &d1, discs const &d2) {
  auto [t1, period1] = d1;
  auto [t2, period2] = d2;
  if (period1 < period2) {
    // Main loop strides with the bigger period
    swap(period1, period2);
    swap(t1, t2);
  }
  // Wait for initial alignment of both groups
  auto t = max(t1, t2);
  // Align with period1
  if ((t - t1) % period1 != 0)
    t += period1 - (t - t1) % period1;
  // Align with period2
  while ((t - t2) % period2 != 0)
    t += period1;
  return { t, lcm(period1, period2) };
}

vector<discs> read(bool extra) {
  vector<discs> result;
  auto add = [&](unsigned num, unsigned a, unsigned m) {
               assert(a < m);
               // Account for time delay to drop to the disc
               a = (a + num) % m;
               // How much more time to first effectively align?
               if (a != 0)
                 a = m - a;
               result.emplace_back(a, m);
             };
  string disc, has, positions, at, time, it, is, position;
  char number, fullstop;
  unsigned num, m, a;
  unsigned max_num = 0;
  while (cin >> disc >> number >> num >> has >> m >> positions >>
         at >> time >> it >> is >> at >> position >> a >> fullstop) {
    add(num, a, m);
    max_num = max(max_num, num);
  }
  if (extra)
    add(max_num + 1, 0, 11);
  return result;
}

void solve(bool extra) {
  auto machine = read(extra);
  discs everything{ 0, 1 };
  while (!machine.empty()) {
    everything = align(everything, machine.back());
    machine.pop_back();
  }
  cout << everything.first << '\n';
}

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
