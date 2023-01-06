// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

// There's likely some clever observation available here.  Certainly
// the initial input and the disk sizes are all related in the actual
// problem (input size happens to be 17, disk sizes are 2^k * 17).
// The dragon part could be represented concisely for sure, but things
// get messy after truncating to the disk size and then through the
// checksum computation.  And anything clever might not be obviously
// workable if the disk and input sizes didn't happen to be carefully
// chosen, which the problem does not imply.  Anyway, the bigger size
// is still small enough that it's likely just not worth thought.
// Using vector<bool> instead of string for the representation is also
// a fairly trivial change to save an easy factor of 8 in memory use.

string dragon(string a) {
  size_t i = a.length();
  a.push_back('0');
  while (i-- > 0)
    a.push_back('0' - a[i] + '1');
  return a;
}

string fill_disk(size_t disk_size, string const &initial) {
  string contents(initial);
  while (contents.size() < disk_size)
    contents = dragon(contents);
  return contents.substr(0, disk_size);
}

string checksum(string const &contents) {
  string checksum = contents;
  assert(checksum.size() % 2 == 0);
  while (checksum.size() % 2 == 0) {
    string nextchk;
    nextchk.reserve(checksum.size() / 2);
    for (size_t i = 0; i + 1 < checksum.size(); i += 2)
      nextchk.push_back(checksum[i] == checksum[i + 1] ? '1' : '0');
    checksum = nextchk;
  }
  return checksum;
}

void solve(size_t disk_size) {
  string initial;
  cin >> initial;
  cout << checksum(fill_disk(disk_size, initial)) << '\n';
}

void part1() { solve(272); }
void part2() { solve(35651584); }

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
