// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <functional>
#include <cctype>
#include <cassert>

using namespace std;

struct computer {
  // Program counter
  unsigned pc;
  // Registers a through d
  array<int, 4> reg;
  // The instructions
  vector<function<void(computer &cptr)>> instr;
  // PC and register file state when output happens
  vector<pair<unsigned, array<int, 4>>> outputs;

  // Read instructions from stdin
  computer();

  // Register (first == true) or immediate (first == false)
  int value(pair<bool, int> const &regimm) const {
    return regimm.first ? reg[regimm.second] : regimm.second;
  }

  // Check for the desired clock signal, and halt immediately if
  // there's a mismatch.  If things are good so far, check for a
  // repeat of state, and halt if a repeat is detected.
  void out(unsigned x);

  // Execute the program starting with some initial value in a.
  // Return whether or not the correct clock signal is being produced.
  bool execute(int rega);
};

computer::computer() {
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
    } else if (op == "out") {
      auto send = regimm();
      instr.push_back([=](computer &cptr) {
                        cptr.out(cptr.value(send));
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

void computer::out(unsigned x) {
  assert(x == 0 || x == 1);
  if (outputs.size() % 2 != x) {
    // Nope
    outputs.clear();
    // This suffices to force a break in execution
    pc = instr.size();
  } else {
    outputs.emplace_back(pc, reg);
    if (outputs.size() < 3)
      // Not enough to judge if repeating yet
      return;
    // See if state is repeating yet.  The output right so far.  Say
    // the last state is index 5 (i.e., size is 6); then a repeat is
    // detected if the pc and registers match those from state 3 or 1.
    for (int i = outputs.size() - 3; i >= 0; i -= 2)
      if (outputs[i] == outputs.back())
        // Repeat found, halt, do not catch fire
        pc = instr.size();
  }
}

bool computer::execute(int rega) {
  reg[0] = rega;
  for (size_t i = 1; i < reg.size(); ++i)
    reg[i] = 0;
  pc = 0;
  outputs.clear();
  // Output will cause a break by setting pc out of range when either
  // the output is incorrect or repeating good state is detected
  while (pc < instr.size())
    instr[pc](*this);
  return !outputs.empty();
}

void part1() {
  computer c;
  int a = 1;
  while (!c.execute(a))
    ++a;
  cout << a << '\n';
}

void part2() {
  cout << "Transmit the Signal!\n";
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
