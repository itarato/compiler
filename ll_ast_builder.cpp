#include <iostream>
#include <algorithm>
#include <iterator>

#include "ll_ast_builder.h"
#include "ast_node.h"

void print_flat_grammar_rules(vector<FlatGrammarRule> fgr) {
  cout << "Grammar / decision list:\n";
  for (auto & rule : fgr) {
    cout << rule.rule_name << " :> " << rule.rule << "( ";
    copy(rule.reached_tokens.begin(), rule.reached_tokens.end(), ostream_iterator<string>(cout, " "));
    cout << ")\n";
  }
}

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t) : grammar(g), tokenizer(t) {
  build_flat_grammar_version();
};

AstNode * LLAstBuilder::build() {
  build_token_decision_matrix();
  print_flat_grammar_rules(flat_grammar);

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
      flat_grammar.push_back({rule_name, rule, find_starting_tokens(rule)});
    }
  }
}

void LLAstBuilder::build_token_decision_matrix() {
  for (unsigned int i = 0; i < flat_grammar.size(); i++) {
    cout << flat_grammar[i].rule_name << endl;
  }
}

vector<string> LLAstBuilder::find_starting_tokens(GrammarRule rule) {
  vector<string> out;
  if (rule.parts.size() == 0) return out;

  if (is_token(rule.parts[0])) {
    out.push_back(rule.parts[0]);
  } else {
    for (auto & rule : grammar->lines[rule.parts[0]].rules) {
      auto sub_out = find_starting_tokens(rule);
      copy(sub_out.begin(), sub_out.end(), back_inserter(out));
    }
  }

  return out;
}
