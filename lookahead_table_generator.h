#pragma once

#include <map>
#include <utility>
#include <vector>
#include "grammar.h"

using namespace std;

class LookaheadTableGenerator {
public:
  map<pair<string, unsigned int>, vector<vector<string>>> rule_lookup;
  Grammar *grammar;
  unsigned int ll_max_level;
  bool verbose_mode;

  LookaheadTableGenerator(Grammar *, unsigned int, bool = false);
  void print_rule_lookup();
  bool generate();

private:
  void build_lookup_table(unsigned int);
  vector<vector<string>> find_starting_tokens(string, unsigned int, int);
  bool is_lookup_table_valid();
};
