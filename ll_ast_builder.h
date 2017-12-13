#pragma once

#include <utility>
#include <vector>
#include <string>

#include "tokenizer.h"
#include "grammar.h"
#include "ast_node.h"

using namespace std;

class LLAstBuilder {
public:
  Grammar *grammar;
  Tokenizer *tokenizer;
  vector<pair<string, GrammarRule>> flat_grammar;

  LLAstBuilder(Grammar *, Tokenizer *);
  AstNode * build();

private:
  void build_flat_grammar_version();
  bool validate_grammar();
  void build_token_decision_matrix();
};
