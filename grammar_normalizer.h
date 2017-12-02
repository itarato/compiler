#pragma once

#include <string>

#include "grammar.h"

using namespace std;

class GrammarNormalizer {
public:
  Grammar *grammar;

  GrammarNormalizer(Grammar*);
  void normalize();

private:
  string get_left_recursive_tag();
  void eliminate_left_recursive();
  void eliminate_premature_match();
  int compare_rules(GrammarRule, GrammarRule);
};
