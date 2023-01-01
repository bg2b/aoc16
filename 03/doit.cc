// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>

using namespace std;

bool is_triangle(int s1, int s2, int s3) {
  return s1 + s2 > s3 && s2 + s3 > s1 && s3 + s1 > s2;
}

void part1() {
  int ans = 0;
  int s1, s2, s3;
  while (cin >> s1 >> s2 >> s3)
    if (is_triangle(s1, s2, s3))
      ++ans;
  cout << ans << '\n';
}

void part2() {
  int ans = 0;
  int s1[3], s2[3], s3[3];
  while (cin >> s1[0] >> s1[1] >> s1[2] >> s2[0] >> s2[1] >> s2[2] >>
         s3[0] >> s3[1] >> s3[2])
    for (int i = 0; i < 3; ++i)
      if (is_triangle(s1[i], s2[i], s3[i]))
        ++ans;
  cout << ans << '\n';
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
