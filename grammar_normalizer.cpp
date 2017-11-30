#include <string>
#include <iostream>

#include "grammar_normalizer.h"
#include "grammar.h"

using namespace std;

GrammarNormalizer::GrammarNormalizer(Grammar *g) : grammar(g) {}

void GrammarNormalizer::normalize() {
  get_left_recursive_tag();
}

string GrammarNormalizer::get_left_recursive_tag() {
  for (auto it = grammar->lines.begin(); it != grammar->lines.end(); it++) {
    cout << it->first << endl;
  }
  return "";
}
