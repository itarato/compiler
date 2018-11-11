//
// Created by itarato on 06/11/18.
//

#ifndef COMPILER_BOTTOMTOPASTBUILDER_H
#define COMPILER_BOTTOMTOPASTBUILDER_H

#include <optional>
#include <variant>
#include "ast_node.h"
#include "grammar.h"
#include "token.h"
#include "tokenizer.h"

using TokenList = vector<variant<Token, string>>;

struct BottomTopAstBuilder {
  vector<pair<string, GrammarRule>> rules;
  Tokenizer *tokenizer;
  bool verbose_mode;

  BottomTopAstBuilder(Grammar *, Tokenizer *, bool = false);
  AstNode *build();

 private:
  optional<size_t> find_match(TokenList &, size_t, size_t);
  TokenList substitute(TokenList *, size_t, size_t, size_t);
  void restore_substitute(TokenList *, size_t, TokenList);
};

#endif  // COMPILER_BOTTOMTOPASTBUILDER_H
