#pragma once

#include <map>
#include <string>
#include <vector>

#include "ast_node.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "token.h"
#include "tokenizer.h"

using namespace std;

struct FlatGrammarRule {
  string rule_name;
  GrammarRule rule;
};

struct LookupGroup {
  vector<vector<string>> token_seqs;
  int idx;
};

void print_flat_grammar_rules(vector<FlatGrammarRule>);

class LLAstBuilder {
public:
  Grammar *grammar;
  Tokenizer *tokenizer;
  unsigned int ll_level;
  vector<FlatGrammarRule> flat_grammar;
  map<string, LookupGroup> rule_lookup;

  LLAstBuilder(Grammar *, Tokenizer *, unsigned int = 1);
  AstNode *build();

private:
  void build_flat_grammar_version();
  vector<vector<string>> find_starting_tokens(string, unsigned int, int);
  void print_rule_lookup();
  int lookup(string, vector<string>);
};
