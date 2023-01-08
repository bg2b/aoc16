// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cctype>
#include <cassert>

using namespace std;

struct computer {
  size_t pc{0};
  vector<int> reg{0, 0, 0, 0};
  vector<function<void(computer &cptr)>> instr;

  // Read instructions from stdin
  computer(int regc);

  // Register (first == true) or immediate (first == false)
  int value(pair<bool, int> const &regimm) const {
    return regimm.first ? reg[regimm.second] : regimm.second;
  }

  void execute();
};

computer::computer(int regc) {
  reg['c' - 'a'] = regc;
  auto regimm =
    []() {
      string operand;
      cin >> operand;
      if (isdigit(operand[0]) || operand[0] == '-')
        return make_pair(false, stoi(operand));
      assert(operand.size() == 1 && operand[0] >= 'a' && operand[0] <= 'd');
      return make_pair(true, operand[0] - 'a');
    };
  auto getreg = [&]() {
                  auto [reg, r] = regimm();
                  assert(reg);
                  return r;
                };
  string op;
  while (cin >> op) {
    if (op == "inc" || op == "dec") {
      int r = getreg();
      int c = op == "inc" ? +1 : -1;
      instr.push_back([=](computer &cptr) {
                        cptr.reg[r] += c;
                        ++cptr.pc;
                      });
    } else if (op == "cpy") {
      auto from = regimm();
      int to = getreg();
      instr.push_back([=](computer &cptr) {
                        cptr.reg[to] = cptr.value(from);
                        ++cptr.pc;
                      });
    } else {
      assert(op == "jnz");
      auto test = regimm();
      int disp;
      cin >> disp;
      instr.push_back([=](computer &cptr) {
                        if (cptr.value(test) != 0)
                          cptr.pc += disp;
                        else
                          ++cptr.pc;
                      });
    }
  }
}

void computer::execute() {
  while (pc < instr.size())
    instr[pc](*this);
}

void solve(int regc) {
  computer c(regc);
  c.execute();
  cout << c.reg[0] << '\n';
}

void part1() { solve(0); }
void part2() { solve(1); }

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
