// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>
#include <optional>
#include <cassert>

using namespace std;

size_t const num_floors = 4;
// I'm using a bitset reprentation for items
using items = unsigned;
using RTF = array<items, num_floors>;

// Elevator is always bit 0
items const elevator = 1;
// Chips are bits 2, 4, ...
items all_chips;
// Generators are bits 1, 3, ...
items all_generators;
// Elevator + all chips + all generators
items everything;
map<string, int> element_nums;

inline items generator(int e) { return 1 << (2 * e + 1); }
inline items chip(int e) { return 1 << (2 * e + 2); }
inline items to_chip(items generators) { return generators << 1; }
inline items to_generator(items chips) { return chips >> 1; }

// Find the index of an element.  If necessary, assign it (and update
// all_chips, all_generators, etc.)
int add_element(string const &element) {
  auto p = element_nums.find(element);
  if (p == element_nums.end()) {
    p = element_nums.emplace(element, element_nums.size()).first;
    all_chips |= chip(p->second);
    all_generators |= generator(p->second);
    everything = all_chips + all_generators + elevator;
  }
  return p->second;
}

// Read the initial state, tacking on the things in extra for the
// ground floor
RTF read(string const &extra) {
  RTF rtf;
  for (size_t floor = 0; floor < rtf.size(); ++floor) {
    rtf[floor] = floor == 0 ? elevator : 0;
    string line;
    getline(cin, line);
    if (floor == 0)
      line += extra;
    string prev;
    stringstream ss(line);
    string token;
    while (ss >> token) {
      if (token.back() == ',' || token.back() == '.')
        token.pop_back();
      if (token == "generator")
        rtf[floor] |= generator(add_element(prev));
      prev = token;
      auto pos = token.find("-compatible");
      if (pos != string::npos)
        rtf[floor] |= chip(add_element(token.substr(0, pos)));
    }
  }
  return rtf;
}

// Search state
struct state {
  // Number of steps to get here
  unsigned steps{0};
  // Where items are
  RTF rtf;

  // Initial state
  state(RTF const &rtf_) : rtf(rtf_) {}

  // Finished?
  bool done() const { return rtf.back() == everything; }
  // Is some chip in the RTF fried?
  bool fried() const;

  // Unfried successor states
  list<state> successors() const;
};

bool state::fried() const {
  for (auto floor : rtf) {
    items chips = floor & all_chips;
    items generators = floor & all_generators;
    items powered = to_chip(to_generator(chips) & generators);
    items unpowered = chips & ~powered;
    // If there are any generators, then either all chips must be
    // powered or something will fry
    if (unpowered && generators)
      return true;
  }
  return false;
}

list<state> state::successors() const {
  list<state> result;
  // Find the elevator
  unsigned from;
  for (from = 0; from < num_floors && !(rtf[from] & elevator); ++from)
    ;
  assert(rtf[from] & elevator);
  // Consider moving some items between from and to.  Add a new
  // successor state if nothing is fried by doing that.
  auto maybe_move =
    [&](items moving, unsigned to) {
      state next(*this);
      assert((next.rtf[from] & moving) == moving);
      assert((next.rtf[to] & moving) == 0);
      next.rtf[from] ^= moving;
      next.rtf[to] ^= moving;
      if (next.fried())
        return;
      ++next.steps;
      result.push_back(next);
    };
  items current = rtf[from];
  for (int dir : { -1, +1 }) {
    if ((from == 0 && dir < 0) || (from == num_floors - 1 && dir > 0))
      continue;
    unsigned to = from + dir;
    for (items item = elevator << 1; item <= current; item <<= 1) {
      if ((current & item) == 0)
        // Item not on this floor
        continue;
      maybe_move(item + elevator, to);
      // Possibly also move a second item
      for (items other = item << 1; other <= current; other <<= 1)
        if ((current & other) != 0)
          maybe_move(item + other + elevator, to);
    }
  }
  return result;
}

// When searching, there's the tendency to generate lots of equivalent
// RTF states.  This function renumbers elements, generating a
// canonical representative for all such states.  E.g., this state:
//
// . .. .. .. .. .. .. .. .. .. ..
// . .. .. .. .. TM .. .. .. .. ..
// . SM SG .. .. .. TG RM RG CM CG
// E .. .. PM PG .. .. .. .. .. ..
//
// is equivalent to this one:
//
// . .. .. .. .. .. .. .. .. .. ..
// . .. .. .. .. .. .. .. .. CM ..
// . .. .. PM PG TM TG RM RG .. CG
// E SM SG .. .. .. .. .. .. .. ..
//
// as far as steps to solution are concerned.  I'll store canonical
// representatives in a table of visited configurations.  If I find a
// state whose canonical representative has already been visited, I
// can toss the state since there is already some other state being
// explored that's equally close to the goal.
//
// The idea is just to scan the floors, looking for powered chips,
// then for isolated chips, then for isolated generators, assigning
// new numbers in the process.  Reordering bits after renumbering is
// not really efficient, but hopefully it's clear.
RTF canonical(RTF const &rtf) {
  int ne = element_nums.size();
  vector<int> renumbering(ne, -1);
  int next = 0;
  for (auto floor : rtf) {
    for (int e = 0; e < ne; ++e) {
      if (renumbering[e] != -1)
        continue;
      items both = chip(e) | generator(e);
      if ((floor & both) == both)
        renumbering[e] = next++;
    }
    for (int e = 0; e < ne; ++e)
      if (renumbering[e] == -1 && (floor & chip(e)) == chip(e))
        renumbering[e] = next++;
    for (int e = 0; e < ne; ++e)
      if (renumbering[e] == -1 && (floor & generator(e)) == generator(e))
        renumbering[e] = next++;
  }
  assert(next == ne);
  RTF result;
  for (size_t floor = 0; floor < num_floors; ++floor) {
    result[floor] = rtf[floor] & elevator;
    for (int e = 0; e < ne; ++e) {
      if (rtf[floor] & chip(e))
        result[floor] |= chip(renumbering[e]);
      if (rtf[floor] & generator(e))
        result[floor] |= generator(renumbering[e]);
    }
  }
  return result;
}

void solve(string const &extra) {
  state start(read(extra));
  // Stores canonical representatives
  set<RTF> seen;
  list<state> frontier;
  auto add = [&](state const &s) {
               RTF rtf = canonical(s.rtf);
               if (seen.find(rtf) != seen.end())
                 return;
               seen.insert(rtf);
               frontier.push_back(s);
             };
  add(start);
  optional<unsigned> steps;
  while (!frontier.empty()) {
    state s = frontier.front();
    frontier.pop_front();
    if (s.done()) {
      steps = s.steps;
      break;
    }
    for (auto const &next : s.successors())
      add(next);
  }
  assert(steps);
  cout << *steps << '\n';
}

void part1() { solve(""); }
void part2() {
  solve(" elerium generator elerium-compatible microchip"
        " dilithium generator dilithium-compatible microchip");
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
