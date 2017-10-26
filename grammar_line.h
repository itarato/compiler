#pragma once

#include <vector>
#include <iostream>

#include "grammar_rule.h"

using namespace std;

class GrammarLine {
public:
  vector<GrammarRule> rules;
  void add_rule(GrammarRule);

  friend ostream & operator<<(ostream & os, const GrammarLine & self) {
    for (auto const rule : self.rules) {
      os << rule << " ";
    }
    return os;
  };
};
