#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "ast_node.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "token.h"
#include "tokenizer.h"

using namespace std;

class LLAstBuilder {
public:
  Grammar *grammar;
  Tokenizer *tokenizer;
  unsigned int ll_max_level;
  map<pair<string, unsigned int>, vector<vector<string>>> rule_lookup;
  bool verbose_mode;

  LLAstBuilder(Grammar *, Tokenizer *, unsigned int = 1, bool = false);
  AstNode *build();

private:
  void build_lookup_table(unsigned int);
  vector<vector<string>> find_starting_tokens(string, unsigned int, int);
  void print_rule_lookup();
  int lookup(string, vector<string>);
  bool is_lookup_table_valid();
};
