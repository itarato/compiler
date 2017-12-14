#pragma once

#include <vector>
#include <string>

#include "tokenizer.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "ast_node.h"
#include "token.h"

using namespace std;

struct FlatGrammarRule {
  string rule_name;
  GrammarRule rule;
  vector<string> reached_tokens;
};
void print_flat_grammar_rules(vector<FlatGrammarRule>);

class LLAstBuilder {
public:
  Grammar *grammar;
  Tokenizer *tokenizer;
  vector<FlatGrammarRule> flat_grammar;

  LLAstBuilder(Grammar *, Tokenizer *);
  AstNode * build();

private:
  void build_flat_grammar_version();
  bool validate_grammar();
  void build_token_decision_matrix();
  vector<string> find_starting_tokens(GrammarRule);
};
