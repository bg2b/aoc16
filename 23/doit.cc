// -*- C++ -*-
// Optimization useful for part 2 (still ~10 seconds, but otherwise 2 min)
// g++ -std=c++17 -Wall -g -O -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cctype>
#include <cassert>

using namespace std;

struct computer;

using instruction = function<void(computer &cptr, int toggled)>;

struct computer {
  size_t pc{0};
  vector<int> reg{0, 0, 0, 0};
  vector<pair<instruction, int>> instr;

  // Read instructions from stdin
  computer(int rega);

  // Register (first == true) or immediate (first == false)
  int value(pair<bool, int> const &regimm) const {
    return regimm.first ? reg[regimm.second] : regimm.second;
  }

  // Toggle an instruction (relative to pc)
  void toggle(int offset);

  void execute();
};

computer::computer(int rega) {
  reg[0] = rega;
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
  auto add = [&](instruction fn) { instr.push_back({ fn, 0 }); };
  string op;
  while (cin >> op) {
    if (op == "inc" || op == "dec") {
      int r = getreg();
      int c = op == "inc" ? +1 : -1;
      add([=](computer &cptr, int toggled) {
            cptr.reg[r] += (toggled % 2 == 0 ? c : -c);
            ++cptr.pc;
          });
    } else if (op == "tgl") {
      int r = getreg();
      add([=](computer &cptr, int toggled) {
            if (!toggled)
              cptr.toggle(cptr.reg[r]);
            else
              // inc or dec
              cptr.reg[r] += (toggled % 2 == 1 ? +1 : -1);
            ++cptr.pc;
          });
    } else if (op == "cpy") {
      auto arg1 = regimm();
      int arg2 = getreg();
      add([=](computer &cptr, int toggled) {
            if (toggled % 2 == 0) {
              // cpy
              cptr.reg[arg2] = cptr.value(arg1);
              ++cptr.pc;
            } else {
              // jnz
              if (cptr.value(arg1) != 0)
                cptr.pc += cptr.reg[arg2];
              else
                ++cptr.pc;
            }
          });
    } else {
      assert(op == "jnz");
      auto arg1 = regimm();
      auto arg2 = regimm();
      add([=](computer &cptr, int toggled) {
            if (toggled % 2 == 0) {
              // jnz
              if (cptr.value(arg1) != 0)
                cptr.pc += cptr.value(arg2);
              else
                ++cptr.pc;
            } else {
              // cpy
              if (!arg2.first)
                // immediate destination
                return;
              cptr.reg[arg2.second] = cptr.value(arg1);
              ++cptr.pc;
            }
          });
    }
  }
}

void computer::toggle(int offset) {
  if (offset < 0 && int(pc) < -offset)
    // Out of bounds
    return;
  size_t index = pc + offset;
  if (index >= instr.size())
    // Out of bounds
    return;
  ++instr[index].second;
}

void computer::execute() {
  while (pc < instr.size())
    instr[pc].first(*this, instr[pc].second);
}

void solve(int rega) {
  computer c(rega);
  c.execute();
  cout << c.reg[0] << '\n';
}

void part1() { solve(7); }
void part2() { solve(12); }

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
