// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <cassert>

using namespace std;

struct contents {
  // Original file contents
  string compressed;
  // Repeat count * repeated hunk, empty = compressed has no markers
  list<pair<size_t, contents>> uncompressed;

  contents(string const &s);

  // level = 0 => size of compressed
  // level = 1 => one step uncompression (part 1)
  size_t uncompressed_size(unsigned level) const;
};

contents::contents(string const &s) : compressed(s) {
  if (compressed.find('(') == string::npos)
    // No further compressed subblocks
    return;
  string prefix;
  auto add_prefix = [&]() {
                      if (!prefix.empty())
                        uncompressed.emplace_back(1, prefix);
                      prefix.clear();
                    };
  stringstream ss(compressed);
  char c;
  while (ss >> c) {
    if (c != '(') {
      prefix.push_back(c);
      continue;
    }
    add_prefix();
    char x, right;
    size_t len, repeats;
    ss >> len >> x >> repeats >> right;
    assert(x == 'x' && right == ')');
    string repeated;
    for (size_t _ = 0; _ < len; ++_)
      repeated.push_back(ss.get());
    uncompressed.emplace_back(repeats, repeated);
  }
  add_prefix();
}

size_t contents::uncompressed_size(unsigned level) const {
  if (level == 0 || uncompressed.empty())
    return compressed.size();
  size_t result = 0;
  for (auto const &block : uncompressed)
    result += block.first * block.second.uncompressed_size(level - 1);
  return result;
}

void decompress(unsigned level) {
  string line;
  getline(cin, line);
  cout << contents(line).uncompressed_size(level) << '\n';
}

void part1() { decompress(1); }
void part2() { decompress(9999); }

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
