#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include <map>

#include "ll_ast_builder.h"
#include "ast_node.h"
#include "token.h"

void print_flat_grammar_rules(vector<FlatGrammarRule> fgr) {
  cout << "Grammar / decision list:\n";
  unsigned int idx = 0;
  for (auto & rule : fgr) {
    cout << "#" << idx++ << " " << rule.rule_name << " :> " << rule.rule << endl;
  }
}

LLAstBuilder::LLAstBuilder(Grammar *g, Tokenizer *t) : grammar(g), tokenizer(t) {
  build_flat_grammar_version();
};

AstNode * LLAstBuilder::build() {
  print_flat_grammar_rules(flat_grammar);

  if (!validate_grammar()) {
    cout << "Invalid grammar for LL parser.\n";
    return nullptr;
  }

  vector<Token> source_tokens(tokenizer->tokens);
  vector<string> rule_tokens({"PROG"});

  return nullptr;
}

bool LLAstBuilder::validate_grammar() {
  // map<string, set<string>> m;
  //
  // for (auto & flat_grammar_rule : flat_grammar) {
  //   if (m.find(flat_grammar_rule.rule_name) == m.end()) {
  //     m[flat_grammar_rule.rule_name] = {};
  //   }
  //
  //   for (auto & token_raw : flat_grammar_rule.reached_tokens) {
  //     if (m[flat_grammar_rule.rule_name].find(token_raw) != m[flat_grammar_rule.rule_name].end()) {
  //       return false;
  //     }
  //     m[flat_grammar_rule.rule_name].insert(token_raw);
  //   }
  // }

  return true;
}

void LLAstBuilder::build_flat_grammar_version() {
  unsigned int idx = 0;
  for (auto & line_pair : grammar->lines) {
    string rule_name(line_pair.first);
    for (auto & rule : line_pair.second.rules) {
      flat_grammar.push_back({rule_name, rule});
      vector<string> reached_tokens = find_starting_tokens(rule);
      for (auto & reached_token : reached_tokens) {
        rule_lookup[rule_name][reached_token] = idx;
      }

      idx++;
    }

  }

  print_rule_lookup();
}

void LLAstBuilder::print_rule_lookup() {
  cout << "Rule lookup matrix:\n";

  for (auto & rule_lookup_pair : rule_lookup) {
    for (auto & token_pair : rule_lookup_pair.second) {
      cout << "[" << rule_lookup_pair.first << ", " << token_pair.first << "] -> " << token_pair.second << endl;
    }
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
