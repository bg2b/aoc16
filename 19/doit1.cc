// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit1 doit1.cc
// ./doit1 1 < input  # part 1
// ./doit1 2 < input  # part 2

#include <iostream>
#include <cassert>

using namespace std;

void part1() {
  // Classical Josephus problem
  unsigned n;
  cin >> n;
  unsigned highest_one_bit = 1;
  while (2 * highest_one_bit <= n)
    highest_one_bit *= 2;
  unsigned l = n - highest_one_bit;
  cout << 2 * l + 1 << '\n';
}

void part2() {
  // The n-cowboy shootout problem, https://oeis.org/A334473
  unsigned n;
  cin >> n;
  unsigned highest_pow3 = 1;
  while (3 * highest_pow3 <= n)
    highest_pow3 *= 3;
  unsigned ans;
  if (n == highest_pow3)
    ans = n;
  else if (n < 2 * highest_pow3)
    ans = n % highest_pow3;
  else
    ans = highest_pow3 + 2 * (n % highest_pow3);
  cout << ans << '\n';
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
