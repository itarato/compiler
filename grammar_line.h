#pragma once

#include <iostream>
#include <vector>

#include "grammar_rule.h"

using namespace std;

class GrammarLine {
 public:
  vector<GrammarRule> rules;
  void add_rule(GrammarRule);

  friend ostream &operator<<(ostream &os, const GrammarLine &self) {
    for (auto it = self.rules.begin(); it != self.rules.end(); it++) {
      os << *it;
      if ((it + 1) != self.rules.end()) os << "| ";
    }
    return os;
  };
};
