#pragma once

#include <string>

#include "grammar.h"

using namespace std;

class GrammarNormalizer {
public:
  Grammar *grammar;

  GrammarNormalizer(Grammar*);
  void normalize();
  string get_left_recursive_tag();
};
