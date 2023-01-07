// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <list>
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

struct ring {
  // elves.front() is the next elf to steal; the list is rotated after
  // each step to maintain this
  list<unsigned> elves;
  // Which elf will get stolen from
  list<unsigned>::iterator victim;

  ring(unsigned n);

  // elves.front() steals from *victim (who is elminated), ring is
  // rotated, next victim chosen
  void steal();

  unsigned winner() const { return elves.size() == 1 ? elves.front() : 0; }
};

ring::ring(unsigned n) {
  assert(n >= 1);
  unsigned first_victim = n / 2 + 1;
  for (unsigned elf = 1; elf <= n; ++elf) {
    elves.push_back(elf);
    if (elf == first_victim)
      victim = prev(elves.end());
  }
}

void ring::steal() {
  // Eliminate victim and tentatively select next (when the number of
  // elves goes from odd to even then two steps are needed, but see
  // end)
  auto next_victim = next(victim);
  elves.erase(victim);
  victim = next_victim;
  // Rotate
  unsigned thief = elves.front();
  elves.pop_front();
  elves.push_back(thief);
  // NB, don't move this prior to the rotate; when going from 3 elves
  // to 2, the pointers will get tangled up (victim would point to the
  // elf that's going to be rotated)
  if (elves.size() % 2 == 0)
    ++victim;
}

// After running some examples and playing around with this, it's
// obvious that there's plenty of regularity related to powers of 3,
// and there should be an analytic answer.  See doit1.cc
void part2() {
  unsigned n;
  cin >> n;
  ring elves(n);
  while (!elves.winner())
    elves.steal();
  cout << elves.winner() << '\n';
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
