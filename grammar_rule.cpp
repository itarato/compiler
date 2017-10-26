#include <vector>
#include <string>
#include <iostream>
#include <iterator>

#include "grammar_rule.h"

using namespace std;

void GrammarRule::add_part(string part) {
  parts.push_back(part);
}
