#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ast_node.h"
#include "grammar.h"
#include "grammar_line.h"
#include "grammar_rule.h"
#include "token.h"
#include "tokenizer.h"

using namespace std;

typedef vector<Token>::iterator vec_iter_t;

class AstBuilder {
 public:
  Grammar *grammar;
  Tokenizer *tokenizer;
  bool verbose_mode;

  AstBuilder(Grammar *, Tokenizer *, bool = false);
  AstNode *build();

 private:
  AstNode *try_grammar_line(GrammarLine *, vec_iter_t *, string);
  AstNode *try_grammar_rule(GrammarRule *, vec_iter_t *, string);
  bool is_token_match(string, Token);
};
