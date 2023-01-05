// -*- C++ -*-
// g++ -std=c++17 -Wall -g -O -o doit doit.cc md5.c
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include "md5.h"

using namespace std;

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

unsigned const nchk = 1000;

struct hasher {
  string salt;
  unsigned md5_reps;
  // State is set up for index as the current hash being considered
  unsigned index{0};
  // Rolling buffer of hashes; hash[index % hash.size()] is the hash
  // for index.  The subsequent hashes to check are at index + 1,
  // index + 2, ... (all modulo hash.size())
  array<string, nchk + 1> hash;
  // The character of the first triple-repeat for each hash
  array<char, nchk + 1> first_triple;

  hasher(string const &salt_, unsigned md5_reps_);

  // Hash index i and update state
  void fill(unsigned i);

  // Move to the next index
  void next() { fill(++index + nchk); }
  // Is the current index a key?
  bool check() const;
};

hasher::hasher(string const &salt_, unsigned md5_reps_) :
  salt(salt_), md5_reps(md5_reps_) {
  for (unsigned i = 0; i < hash.size(); ++i)
    fill(i);
}

void hasher::fill(unsigned i) {
  string s = salt + to_string(i);
  for (unsigned _ = 0; _ < md5_reps; ++_)
    s = md5(s);
  unsigned ii = i % hash.size();
  hash[ii] = s;
  first_triple[ii] = '\0';
  for (unsigned j = 0; j + 2 < s.length(); ++j)
    if (s[j] == s[j + 1] && s[j] == s[j + 2]) {
      first_triple[ii] = s[j];
      break;
    }
}

bool hasher::check() const {
  unsigned i = index % hash.size();
  if (!first_triple[i])
    return false;
  string repeated(5, first_triple[i]);
  for (unsigned j = 0; j < nchk; ++j) {
    unsigned jj = (index + 1 + j) % hash.size();
    if (first_triple[jj] && hash[jj].find(repeated) != string::npos)
      return true;
  }
  return false;
}

void find_keys(unsigned md5_reps) {
  string salt;
  cin >> salt;
  hasher hsh(salt, md5_reps);
  for (int keys = 0; !(hsh.check() && ++keys >= 64); hsh.next())
    ;
  cout << hsh.index << '\n';
}

void part1() { find_keys(1); }
void part2() { find_keys(2017); }

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
