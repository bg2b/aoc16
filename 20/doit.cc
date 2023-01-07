// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <cassert>

using namespace std;

using IP = uint32_t;

// First element is <= second, single number is [i, i] as opposed to
// the usual C++ style of [i, i+1).  (Note that this is necessary
// since i+1 would potentially wrap, i.e., it wouldn't be possible to
// represent the a range ending at the maximum IP address)
using range = pair<IP, IP>;

// Call a vector of ranges "disjoint" if the ranges are sorted and not
// abutting or overlapping; when [i1, j1] appears before [i2, j2],
// then i2 > j1 + 1.  Note that i2 = j1 + 1 would imply that there
// should instead be one range [i1, j2].
//
// Compute the union of disjoint vectors, giving a new disjoint
// vector.  Basically like a merge sort.
vector<range> onion(vector<range> const &rs1, vector<range> const &rs2) {
  vector<range> result;
  auto add = [&](range const &r) {
               // Careful, want result.back().second + 1 < r.first,
               // but written naively, the + 1 could cause a wrap
               if (result.empty() || size_t(result.back().second) + 1 < r.first)
                 result.push_back(r);
               else
                 result.back().second = max(result.back().second, r.second);
             };
  size_t i1 = 0, i2 = 0;
  while (i1 < rs1.size() && i2 < rs2.size())
    if (rs1[i1].first < rs2[i2].first)
      add(rs1[i1++]);
    else
      add(rs2[i2++]);
  while (i1 < rs1.size())
    add(rs1[i1++]);
  while (i2 < rs2.size())
    add(rs2[i2++]);
  return result;
}

using namespace std;

// Input is some individual blocked ranges, output is a vector of
// disjoint ranges
vector<range> merge(vector<range>::const_iterator start,
                    vector<range>::const_iterator end) {
  assert(start != end);
  size_t n = end - start;
  if (n == 1)
    return { *start };
  // Split, recurse, merge subparts
  auto rs1 = merge(start, start + n / 2);
  auto rs2 = merge(start + n / 2, end);
  return onion(rs1, rs2);
}

vector<range> get_combined() {
  vector<range> blocked;
  IP start, end;
  char dash;
  while (cin >> start >> dash >> end)
    blocked.emplace_back(start, end);
  // Try to help out the merge a little by pre-ordering
  sort(blocked.begin(), blocked.end());
  blocked = merge(blocked.begin(), blocked.end());
  return merge(blocked.begin(), blocked.end());
}

void part1() {
  range first = get_combined().front();
  IP ans = first.first == 0 ? first.second + 1 : 0;
  cout << ans << '\n';
}

void part2() {
  auto combined = get_combined();
  size_t disallowed = 0;
  for (auto const &r : combined)
    disallowed += size_t(r.second) - r.first + 1;
  cout << size_t(numeric_limits<IP>::max()) + 1 - disallowed << '\n';
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
