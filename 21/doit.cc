// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <cassert>

using namespace std;

void swap_letters(string &s, char c1, char c2) {
  auto p1 = s.find(c1);
  auto p2 = s.find(c2);
  assert(p1 != string::npos && p2 != string::npos);
  swap(s[p1], s[p2]);
}

string &rotate(string &s, unsigned count, bool left) {
  count %= s.length();
  if (left)
    rotate(s.begin(), s.begin() + count, s.end());
  else
    rotate(s.rbegin(), s.rbegin() + count, s.rend());
  return s;
}

void rot_based_on(string &s, char c, bool rev) {
  if (rev) {
    // This one is a bit tricky to reverse; just brute force and find
    // the one that matches when going forward
    unsigned count;
    for (count = 0; count < s.length(); ++count) {
      string s1 = s;
      rot_based_on(rotate(s1, count, true), c, false);
      if (s1 == s)
        break;
    }
    rotate(s, count, true);
    return;
  }
  auto p = s.find(c);
  assert(p != string::npos);
  rotate(s, p + 1 + (p >= 4 ? 1 : 0), false);
}

void move_from_to(string &s, unsigned from, unsigned to, bool rev) {
  if (rev)
    swap(from, to);
  char c = s[from];
  s.erase(from, 1);
  s.insert(to, 1, c);
}

using scrambler_op = function<void(string &s, bool rev)>;

struct scrambler {
  vector<scrambler_op> instr;

  scrambler();

  string scramble(string s) const;
  string descramble(string s) const;
};

scrambler::scrambler() {
  auto add = [&](scrambler_op fn) { instr.push_back(fn); };
  string cmd, _;
  while (cin >> cmd) {
    if (cmd == "reverse") {
      unsigned start, end;
      cin >> _ >> start >> _ >> end;
      add([=](string &s, bool) {
            reverse(s.begin() + start, s.begin() + end + 1);
          });
    } else if (cmd == "rotate") {
      string dir;
      cin >> dir;
      if (dir == "left" || dir == "right") {
        unsigned steps;
        cin >> steps >> _;
        bool left = dir == "left";
        add([=](string &s, bool rev) { rotate(s, steps, rev ? !left : left); });
      } else {
        assert(dir == "based");
        char c;
        cin >> _ >> _ >> _ >> _ >> c;
        assert(_ == "letter");
        add([=](string &s, bool rev) { rot_based_on(s, c, rev); });
      }
    } else if (cmd == "move") {
      unsigned from, to;
      cin >> _ >> from >> _ >> _ >> to;
      add([=](string &s, bool rev) { move_from_to(s, from, to, rev); });
    } else {
      assert(cmd == "swap");
      string what;
      cin >> what;
      if (what == "position") {
        unsigned p1, p2;
        cin >> p1 >> _ >> _ >> p2;
        add([=](string &s, bool) { swap(s[p1], s[p2]); });
      } else {
        assert(what == "letter");
        char c1, c2;
        cin >> c1 >> _ >> _ >> c2;
        add([=](string &s, bool) { swap_letters(s, c1, c2); });
      }
    }
  }
}

string scrambler::scramble(string s) const {
  for (auto const &fn : instr)
    fn(s, false);
  return s;
}

string scrambler::descramble(string s) const {
  for (auto i = instr.rbegin(); i != instr.rend(); ++i)
    (*i)(s, true);
  return s;
}

void part1() { cout << scrambler().scramble("abcdefgh") << '\n'; }
void part2() { cout << scrambler().descramble("fbgdceah") << '\n'; }

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
