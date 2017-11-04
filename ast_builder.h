#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "token.h"
#include "tokenizer.h"
#include "grammar.h"
#include "grammar_rule.h"
#include "grammar_line.h"
#include "ast_node.h"

using namespace std;

typedef vector<Token>::iterator vec_iter_t;

class AstBuilder {
public:
  Grammar *grammar;
  Tokenizer *tokenizer;

  AstBuilder(Grammar *, Tokenizer *);
  bool build();

private:
  AstNode * try_grammar_line(GrammarLine *, vec_iter_t *);
  AstNode * try_grammar_rule(GrammarRule *, vec_iter_t *);
  bool is_token(string);
  bool is_token_match(string, Token);
};
