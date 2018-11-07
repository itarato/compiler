//
// Created by itarato on 06/11/18.
//

#include "bottom_top_ast_builder.h"

BottomTopAstBuilder::BottomTopAstBuilder(Grammar *grammar, Tokenizer *tokenizer,
                                         bool verbose_mode)
    : grammar(grammar), tokenizer(tokenizer), verbose_mode(verbose_mode) {}

AstNode *BottomTopAstBuilder::build() { return nullptr; }
