#include <iostream>

#include "ll_ast_builder.h"
#include "ast_node.h"

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t) : grammar(g), tokenizer(t) {
  build_flat_grammar_version();
};

AstNode * LLAstBuilder::build() {
  if (!validate_grammar()) {
    cout << "Invalid grammar for LL parser.\n";
    return nullptr;
  }

  return nullptr;
}

bool LLAstBuilder::validate_grammar() {
  return false;
}

void LLAstBuilder::build_flat_grammar_version() {
  for (auto & line_pair : grammar->lines) {
    string rule_name(line_pair.first);
    for (auto & rule : line_pair.second.rules) {
      flat_grammar.push_back({rule_name, rule});
    }
  }
}
