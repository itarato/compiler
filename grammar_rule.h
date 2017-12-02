#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iterator>

using namespace std;

class GrammarRule {
public:
  vector<string> parts;
  bool is_one_or_more;
  void add_part(string);

  friend ostream & operator<<(ostream & os, const GrammarRule & self) {
    copy(self.parts.begin(), self.parts.end(), ostream_iterator<string>(os, " "));
    return os;
  }
};
