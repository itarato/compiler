#pragma once

#include <map>
#include <iostream>
#include <string>

#include "grammar_line.h"

using namespace std;

class Grammar {
public:
  map<string, GrammarLine> lines;

  Grammar();

  bool is_line_def(string);
  bool is_rule_divider(string);
  void save_grammar_line(GrammarLine *, string);

  friend ostream & operator<<(ostream & os, const Grammar & self) {
    for (auto const line : self.lines) {
      os << line.first << ": " << line.second << endl;
    }
    return os;
  };
};
