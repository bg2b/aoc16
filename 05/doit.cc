// -*- C++ -*-
// g++ -std=c++17 -Wall -g -O -o doit doit.cc md5.c
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include <functional>
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

void search(string const &prefix, function<bool(string const &sig)> found) {
  string door_id;
  cin >> door_id;
  for (unsigned i = 0; ; ++i) {
    string sig = md5(door_id + to_string(i));
    if (sig.substr(0, prefix.length()) == prefix && found(sig))
      return;
  }
}

void part1() {
  string passwd;
  search("00000",
         [&](string const &sig) {
           passwd.push_back(sig[5]);
           return passwd.length() >= 8;
         });
  cout << passwd << '\n';
}

void part2() {
  string bol = "\033[0K\r";
  string uncolored = "\033[0m";
  string red = "\033[31m";
  string green = "\033[32m";
  string passwd = "________";
  unsigned num_found = 0;
  unsigned next_rand = 73;
  search("0000",
         [&](string const &sig) {
           // ...
           cout << bol << uncolored << "Cracking ";
           for (char c : passwd)
             if (c == '_') {
               next_rand += 97;
               char mixed = next_rand % 36;
               if (mixed < 10)
                 mixed = '0' + mixed;
               else
                 mixed = 'a' + (mixed - 10);
               cout << red << mixed;
             } else
               cout << green << c;
           cout << uncolored << flush;
           if (sig.substr(0, 5) != "00000")
             // Just for display purposes
             return false;
           int pos = sig[5] - '0';
           if (pos < 0 || pos > 7 || passwd[pos] != '_')
             // Invalid position or already found
             return false;
           passwd[pos] = sig[6];
           ++num_found;
           return num_found >= 8;
         });
  cout << bol << uncolored << "Password " <<
    green << passwd << uncolored << '\n';
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
