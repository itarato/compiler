#pragma once

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

struct GrammarRule {
  vector<string> parts;
  bool is_one_or_more;
  void add_part(string);

  friend ostream& operator<<(ostream& os, const GrammarRule& self) {
    copy(self.parts.begin(), self.parts.end(),
         ostream_iterator<string>(os, " "));
    return os;
  }
};
