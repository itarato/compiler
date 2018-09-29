#pragma once

#include <iostream>
#include <iterator>
#include <map>
#include <string>

#include "grammar_line.h"

using namespace std;

struct Grammar {
  map<string, GrammarLine> lines;

  explicit Grammar(istream_iterator<string>);

  bool is_line_def(string);
  bool is_rule_divider(string);
  void save_grammar_line(GrammarLine *, string);

  friend ostream &operator<<(ostream &os, const Grammar &self) {
    for (auto const & line : self.lines) {
      os << line.first << ": " << line.second << endl;
    }
    return os;
  };
};

Grammar new_grammar_from_filename(char *);
Grammar new_grammar_from_string(string);
