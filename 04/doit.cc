// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cassert>

using namespace std;

vector<pair<string, int>> filter() {
  vector<pair<string, int>> result;
  string room;
  while (cin >> room) {
    auto sector_pos = room.find_first_of("0123456789");
    assert(sector_pos != string::npos);
    int sector = atoi(room.c_str() + sector_pos);
    assert(room.length() >= 7 && room[room.length() - 7] == '[');
    string checksum(room.end() - 6, room.end() - 1);
    assert(checksum.length() == 5);
    string name(room.begin(), room.begin() + sector_pos - 1);
    vector<pair<size_t, char>> counts;
    for (char c = 'a'; c <= 'z'; ++c)
      counts.emplace_back(count(name.begin(), name.end(), c), c);
    sort(counts.begin(), counts.end(),
         [](auto const &p1, auto const &p2) {
           if (p1.first != p2.first)
             return p1.first > p2.first;
           return p1.second < p2.second;
         });
    string expected;
    for (size_t i = 0; i < 5; ++i)
      expected.push_back(counts[i].second);
    if (checksum == expected)
      result.emplace_back(name, sector);
  }
  return result;
}

void part1() {
  int ans = 0;
  for (auto const &room_and_sector : filter())
    ans += room_and_sector.second;
  cout << ans << '\n';
}

void part2() {
  for (auto const &room_and_sector : filter()) {
    int shift = room_and_sector.second;
    string decrypted;
    for (char c : room_and_sector.first)
      decrypted.push_back(c == '-' ? ' ' : 'a' + (c - 'a' + shift) % 26);
    if (decrypted == "northpole object storage")
      cout << room_and_sector.second << '\n';
  }
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
