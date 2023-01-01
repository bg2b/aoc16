// -*- C++ -*-
// g++ -std=c++17 -Wall -g -o doit doit.cc
// ./doit 1 < input  # part 1
// ./doit 2 < input  # part 2

#include <iostream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

pair<vector<string>, vector<string>> split(string s) {
  vector<string> non_hyper;
  vector<string> hyper;
  while (!s.empty()) {
    auto hyper_start = s.find('[');
    if (hyper_start == string::npos) {
      non_hyper.push_back(s);
      break;
    }
    auto hyper_end = s.find(']', hyper_start + 1);
    auto hyper_len = hyper_end - hyper_start - 1;
    non_hyper.push_back(s.substr(0, hyper_start));
    hyper.push_back(s.substr(hyper_start + 1, hyper_len));
    s = s.substr(hyper_end + 1);
  }
  return { non_hyper, hyper };
}

bool has_abba(string const &s) {
  for (size_t i = 0; i + 3 < s.length(); ++i)
    if (s[i] == s[i + 3] && s[i + 1] == s[i + 2] && s[i] != s[i + 1])
      return true;
  return false;
}

bool supports_tls(string const &s) {
  auto [non_hyper, hyper] = split(s);
  for (auto const &s : hyper)
    if (has_abba(s))
      return false;
  for (auto const &s : non_hyper)
    if (has_abba(s))
      return true;
  return false;
}

bool supports_ssl(string const &s) {
  auto [non_hyper, hyper] = split(s);
  for (auto const &nh : non_hyper)
    for (size_t i = 0; i + 2 < nh.length(); ++i)
      if (nh[i] == nh[i + 2] && nh[i] != nh[i + 1]) {
        string aba = { nh[i + 1], nh[i], nh[i + 1] };
        for (auto const &h: hyper)
          if (h.find(aba) != string::npos)
            return true;
      }
  return false;
}

void count(function<bool(string const &s)> feature) {
  int ans = 0;
  string line;
  while (cin >> line)
    if (feature(line))
      ++ans;
  cout << ans << '\n';
}

void part1() { count(supports_tls); }
void part2() { count(supports_ssl); }

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
