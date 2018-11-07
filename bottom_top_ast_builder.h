//
// Created by itarato on 06/11/18.
//

#ifndef COMPILER_BOTTOMTOPASTBUILDER_H
#define COMPILER_BOTTOMTOPASTBUILDER_H

#include "grammar.h"
#include "tokenizer.h"
#include "ast_node.h"

struct BottomTopAstBuilder {
  Grammar *grammar;
  Tokenizer *tokenizer;
  bool verbose_mode;

  BottomTopAstBuilder(Grammar *, Tokenizer *, bool = false);
  AstNode *build();
};

#endif // COMPILER_BOTTOMTOPASTBUILDER_H
