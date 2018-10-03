#pragma once

#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <memory>

#include "grammar_line.h"

using namespace std;

struct Grammar {
  map<string, unique_ptr<GrammarLine>> lines;

  explicit Grammar(istream_iterator<string>);

  bool is_line_def(string);
  bool is_rule_divider(string);
  void save_grammar_line(unique_ptr<GrammarLine>, string);

  friend ostream &operator<<(ostream &os, const Grammar &self) {
    for (auto const & line : self.lines) {
      os << line.first << ": " << *line.second << endl;
    }
    return os;
  };
};

Grammar new_grammar_from_filename(char *);
Grammar new_grammar_from_string(string);
