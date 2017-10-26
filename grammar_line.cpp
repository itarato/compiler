#include "grammar_line.h"
#include "grammar_rule.h"

void GrammarLine::add_rule(GrammarRule grammar_rule) { 
  rules.push_back(grammar_rule);
};
