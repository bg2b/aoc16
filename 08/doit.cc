// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <cassert>

using namespace std;

unsigned const width = 50;
unsigned const height = 6;

struct screen {
  array<string, height> pixels;

  screen();

  void rect(unsigned w, unsigned h);
  void rotate_row(unsigned y, unsigned b);
  void rotate_col(unsigned x, unsigned b);

  unsigned num_lit() const;
  void show() const;
};

screen::screen() {
  for (unsigned r = 0; r < height; ++r)
    pixels[r] = string(width, '.');
}

void screen::rect(unsigned w, unsigned h) {
  assert(w <= width && h <= height);
  for (unsigned r = 0; r < h; ++r)
    for (unsigned c = 0; c < w; ++c)
      pixels[r][c] = '@';
}

string rotate(string const &s, unsigned b) {
  unsigned l = s.length();
  string new_s;
  for (unsigned i = 0; i < l; ++i)
    new_s.push_back(s[(i + (l - b)) % l]);
  return new_s;
}

void screen::rotate_row(unsigned y, unsigned b) {
  assert(y < height);
  pixels[y] = rotate(pixels[y], b);
}

void screen::rotate_col(unsigned x, unsigned b) {
  assert(x < width);
  string col;
  for (unsigned r = 0; r < height; ++r)
    col.push_back(pixels[r][x]);
  col = rotate(col, b);
  for (unsigned r = 0; r < height; ++r)
    pixels[r][x] = col[r];
}

void screen::show() const {
  for (unsigned r = 0; r < height; ++r)
    cout << pixels[r] << '\n';
}

unsigned screen::num_lit() const {
  unsigned result = 0;
  for (unsigned r = 0; r < height; ++r)
    result += count(pixels[r].begin(), pixels[r].end(), '@');
  return result;
}

screen execute() {
  screen display;
  string instr;
  while (cin >> instr) {
    if (instr == "rect") {
      unsigned w, h;
      char x;
      cin >> w >> x >> h;
      display.rect(w, h);
      continue;
    }
    assert(instr == "rotate");
    char xy, equals;
    unsigned coord, b;
    string by;
    cin >> instr >> xy >> equals >> coord >> by >> b;
    assert((xy == 'x' || xy == 'y') && equals == '=');
    if (instr == "row")
      display.rotate_row(coord, b);
    else
      display.rotate_col(coord, b);
  }
  return display;
}

void part1() { cout << execute().num_lit() << '\n'; }
void part2() { execute().show(); }

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
